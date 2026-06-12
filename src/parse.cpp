// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/parse.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/validation.hpp>

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <vector>

namespace mechanism_configuration
{
  std::expected<Version, Errors> GetVersion(const std::filesystem::path& config_path)
  {
    if (!std::filesystem::exists(config_path))
    {
      return std::unexpected(
          Errors{ { ErrorCode::FileNotFound, mc_fmt::format("Configuration file '{}' does not exist.", config_path.string()) } });
    }

    YAML::Node object;
    try
    {
      object = YAML::LoadFile(config_path.string());
    }
    catch (const YAML::Exception& e)
    {
      return std::unexpected(
          Errors{ { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse '{}': {}", config_path.string(), e.what()) } });
    }

    if (!object[validation::version])
    {
      return std::unexpected(
          Errors{ { ErrorCode::MissingVersionField, mc_fmt::format("The version field was not found in '{}'.", config_path.string()) } });
    }

    return Version(object[validation::version].as<std::string>());
  }

  // Validates and builds a v1 mechanism from an already-loaded YAML node.
  // `read_from_config_file` controls whether validation errors are prefixed with the config path.
  static std::expected<Mechanism, Errors> ParseV1Node(const YAML::Node& object, bool read_from_config_file)
  {
    v1::Parser parser;
    auto errors = parser.Validate(object, read_from_config_file);
    if (!errors.empty())
    {
      return std::unexpected(std::move(errors));
    }
    return parser.Parse(object);
  }

  std::expected<Mechanism, Errors> parse(const std::filesystem::path& config_path)
  {
    // Version-0 mechanisms may be provided as a directory of CAMP files.
    if (std::filesystem::exists(config_path) && std::filesystem::is_directory(config_path))
    {
      return v0::Parser{}.Parse(config_path);
    }

    auto version = GetVersion(config_path);
    if (!version)
    {
      // A missing version field indicates a legacy version-0 configuration. This is
      // temporary and fragile, but required to keep supporting version-0 files.
      for (const auto& [code, message] : version.error())
      {
        if (code == ErrorCode::MissingVersionField)
        {
          return v0::Parser{}.Parse(config_path);
        }
      }
      return std::unexpected(version.error());
    }

    switch (version->major)
    {
      case 0:
        return v0::Parser{}.Parse(config_path);
      case 1:
      {
        v1::Parser parser;
        try
        {
          YAML::Node object = parser.FileToYaml(config_path);
          auto errors = parser.Validate(object);
          if (!errors.empty())
          {
            return std::unexpected(std::move(errors));
          }
          return parser.Parse(object);
        }
        catch (const std::exception& e)
        {
          return std::unexpected(
              Errors{ { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse '{}': {}", config_path.string(), e.what()) } });
        }
      }
      default:
        return std::unexpected(
            Errors{ { ErrorCode::InvalidVersion, mc_fmt::format("Unsupported version number '{}'.", version->to_string()) } });
    }
  }

  std::expected<Mechanism, Errors> parse(const std::string& content)
  {
    YAML::Node object;
    try
    {
      object = YAML::Load(content);
    }
    catch (const std::exception& e)
    {
      return std::unexpected(Errors{ { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse content: {}", e.what()) } });
    }

    if (!object[validation::version])
    {
      return std::unexpected(Errors{ { ErrorCode::MissingVersionField, "The version field was not found." } });
    }

    Version version(object[validation::version].as<std::string>());
    switch (version.major)
    {
      case 1:
        return ParseV1Node(object, /*read_from_config_file=*/false);
      default:
        return std::unexpected(
            Errors{ { ErrorCode::InvalidVersion, mc_fmt::format("Unsupported version number '{}' for string input.", version.to_string()) } });
    }
  }

}  // namespace mechanism_configuration

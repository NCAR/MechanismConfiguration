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
          // Resolve any file-list sections (v1.1+) into a single inline node first.
          auto object = parser.ResolveFileConfig(config_path);
          if (!object)
          {
            return std::unexpected(std::move(object.error()));
          }
          auto errors = parser.Validate(*object);
          if (!errors.empty())
          {
            return std::unexpected(std::move(errors));
          }
          return parser.Parse(*object);
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

}  // namespace mechanism_configuration

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/parse.hpp>

#include "detail/v0/parser.hpp"
#include "detail/v1/parser.hpp"
#include "detail/validation_keys.hpp"

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

    if (std::filesystem::exists(config_path) && std::filesystem::is_directory(config_path)) {
      return Version(0, 0, 0);
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
      // assume it's a v0 config
      return Version(0, 0, 0);
    }

    return Version(object[validation::version].as<std::string>());
  }

  std::expected<Mechanism, Errors> parse(const std::filesystem::path& config_path)
  {
    auto version = GetVersion(config_path);

    if (!version)
    {
      return std::unexpected(std::move(version.error()));
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

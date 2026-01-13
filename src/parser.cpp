// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/mechanism.hpp>
#include <mechanism_configuration/development/validation.hpp>
#include <mechanism_configuration/error_location.hpp>
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/parser.hpp>
#include <mechanism_configuration/parser_result.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v1/parser.hpp>

#include <filesystem>
#include <format>
#include <memory>
#include <vector>

namespace mechanism_configuration
{

  VersionInfo UniversalParser::GetVersion(const std::filesystem::path& config_path)
  {
    VersionInfo info;

    if (!std::filesystem::exists(config_path))
    {
      info.errors.push_back(
          { ConfigParseStatus::FileNotFound, std::format("Configuration file '{}' does not exist.", config_path.string()) });
      return info;
    }

    YAML::Node object;
    try
    {
      object = YAML::LoadFile(config_path.string());
    }
    catch (const YAML::Exception& e)
    {
      info.errors.push_back(
          { ConfigParseStatus::UnexpectedError, std::format("Failed to parse '{}': {}", config_path.string(), e.what()) });
      return info;
    }

    if (!object[development::validation::version])
    {
      info.errors.push_back({ ConfigParseStatus::MissingVersionField,
                              std::format("The version field was not found in '{}'.", config_path.string()) });
      return info;
    }

    Version version = Version(object[development::validation::version].as<std::string>());
    info.version = version.major;

    return info;
  }

  ParserResult<GlobalMechanism> UniversalParser::Parse(const std::filesystem::path& config_path)
  {
    ParserResult<GlobalMechanism> result;

    // Check if the path is a directory
    // If it's a directory, use the v0 parser without version checking
    if (std::filesystem::exists(config_path) && std::filesystem::is_directory(config_path))
    {
      v0::Parser v0_parser;
      auto parser_result = v0_parser.Parse(config_path);
      if (parser_result.errors.empty())
      {
        result.mechanism = std::move(parser_result.mechanism);
      }
      else
      {
        result.errors.insert(result.errors.end(), parser_result.errors.begin(), parser_result.errors.end());
      }
      return result;
    }

    // For files, check the version
    VersionInfo version_info = GetVersion(config_path);
    if (!version_info.errors.empty())
    {
      // If the version field is missing, the parser defaults to version 0.
      // This approach is temporary and fragile, but is required to support
      // the version 0 configurations. It can be removed once version 0 is fully
      // deprecated or no longer supported.
      for (auto& [status, message] : version_info.errors)
      {
        if (status == ConfigParseStatus::MissingVersionField)
        {
          v0::Parser v0_parser;
          auto parser_result = v0_parser.Parse(config_path);
          if (parser_result.errors.empty())
          {
            result.mechanism = std::move(parser_result.mechanism);
          }
          else
          {
            result.errors.insert(result.errors.end(), parser_result.errors.begin(), parser_result.errors.end());
          }
          return result;
        }
      }
      result.errors.insert(result.errors.end(), version_info.errors.begin(), version_info.errors.end());

      return result;
    }

    constexpr unsigned int DEV_VERSION = 2;
    constexpr unsigned int V1_VERSION = 1;

    if (version_info.version == DEV_VERSION)
    {
      development::Parser parser;
      YAML::Node object = parser.FileToYaml(config_path);
      auto validation_errors = parser.Validate(object);
      if (validation_errors.empty())
      {
        result.mechanism = std::make_unique<development::types::Mechanism>(parser.Parse(object));
      }
      else
      {
        result.errors.insert(result.errors.end(), validation_errors.begin(), validation_errors.end());
      }
      return result;
    }
    else if (version_info.version == V1_VERSION)
    {
      v1::Parser v1_parser;
      auto parser_result = v1_parser.Parse(config_path);
      if (parser_result.errors.empty())
      {
        result.mechanism = std::move(parser_result.mechanism);
      }
      else
      {
        result.errors.insert(result.errors.end(), parser_result.errors.begin(), parser_result.errors.end());
      }
      return result;
    }
    else
    {
      std::string message = std::format(
          "error: The supported versions are '{}', '{}' but the invalid version number '{}' was found: '{}'.",
          DEV_VERSION,
          V1_VERSION,
          version_info.version,
          config_path.string());
      result.errors.push_back({ ConfigParseStatus::InvalidVersion, message });
    }

    return result;
  }

}  // namespace mechanism_configuration
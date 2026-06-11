// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/parse.hpp>
// #include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/validation.hpp>

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <memory>
#include <vector>

namespace mechanism_configuration
{
  std::expected<Version, Errors> GetVersion(const std::filesystem::path& config_path)
  {
    if (!std::filesystem::exists(config_path))
    {
      return std::unexpected(Errors{ { ErrorCode::FileNotFound, mc_fmt::format("Configuration file '{}' does not exist.", config_path.string()) } });
    }

    YAML::Node object;
    try
    {
      object = YAML::LoadFile(config_path.string());
    }
    catch (const YAML::Exception& e)
    {
      return std::unexpected(Errors{ { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse '{}': {}", config_path.string(), e.what()) } });
    }

    if (!object[validation::version])
    {
      return std::unexpected(Errors{ { ErrorCode::MissingVersionField, mc_fmt::format("The version field was not found in '{}'.", config_path.string()) } });
    }

    return Version(object[validation::version].as<std::string>());
  }

  void f() {
    // // Check if the path is a directory
    // // If it's a directory, use the v0 parser without version checking
    // if (std::filesystem::exists(config_path) && std::filesystem::is_directory(config_path))
    // {
    //   v0::Parser v0_parser;
    //   auto parser_result = v0_parser.Parse(config_path);
    //   if (parser_result.errors.empty())
    //   {
    //     result.mechanism = std::move(parser_result.mechanism);
    //   }
    //   else
    //   {
    //     result.errors.insert(result.errors.end(), parser_result.errors.begin(), parser_result.errors.end());
    //   }
    //   return result;
    // }

    // // For files, check the version
    // VersionInfo version_info = GetVersion(config_path);
    // if (!version_info.errors.empty())
    // {
    //   // If the version field is missing, the parser defaults to version 0.
    //   // This approach is temporary and fragile, but is required to support
    //   // the version 0 configurations. It can be removed once version 0 is fully
    //   // deprecated or no longer supported.
    //   for (auto& [status, message] : version_info.errors)
    //   {
    //     if (status == ErrorCode::MissingVersionField)
    //     {
    //       v0::Parser v0_parser;
    //       auto parser_result = v0_parser.Parse(config_path);
    //       if (parser_result.errors.empty())
    //       {
    //         result.mechanism = std::move(parser_result.mechanism);
    //       }
    //       else
    //       {
    //         result.errors.insert(result.errors.end(), parser_result.errors.begin(), parser_result.errors.end());
    //       }
    //       return result;
    //     }
    //   }
    //   result.errors.insert(result.errors.end(), version_info.errors.begin(), version_info.errors.end());

    //   return result;
    // }

    // constexpr unsigned int DEV_VERSION = 2;
    // constexpr unsigned int V1_VERSION = 1;

    // if (version_info.version == DEV_VERSION)
    // {
    //   development::Parser parser;
    //   YAML::Node object = parser.FileToYaml(config_path);
    //   auto validation_errors = parser.Validate(object);
    //   if (validation_errors.empty())
    //   {
    //     result.mechanism = std::make_unique<development::types::Mechanism>(parser.Parse(object));
    //   }
    //   else
    //   {
    //     result.errors.insert(result.errors.end(), validation_errors.begin(), validation_errors.end());
    //   }
    //   return result;
    // }
    // else if (version_info.version == V1_VERSION)
    // {
    //   v1::Parser v1_parser;
    //   auto parser_result = v1_parser.Parse(config_path);
    //   if (parser_result.errors.empty())
    //   {
    //     result.mechanism = std::move(parser_result.mechanism);
    //   }
    //   else
    //   {
    //     result.errors.insert(result.errors.end(), parser_result.errors.begin(), parser_result.errors.end());
    //   }
    //   return result;
    // }
    // else
    // {
    //   std::string message = mc_fmt::format(
    //       "error: The supported versions are '{}', '{}' but the invalid version number '{}' was found: '{}'.",
    //       DEV_VERSION,
    //       V1_VERSION,
    //       version_info.version,
    //       config_path.string());
    //   result.errors.push_back({ ConfigParseStatus::InvalidVersion, message });
    // }
  }

  std::expected<Mechanism, Errors> parse(const std::filesystem::path& config_path)
  {
    auto version = GetVersion(config_path);
    if (!version)    {
      return std::unexpected(version.error());
    }
    else {
      switch (version->major)
      {
      case 0:
        throw std::runtime_error("Version 0");
        break;
      case 1:
        throw std::runtime_error("Version 1");
        break;
      }
    }

    return std::unexpected(Errors{ { ErrorCode::InvalidVersion, mc_fmt::format("Unsupported version number '{}'.", version->to_string()) } });
  }

}  // namespace mechanism_configuration
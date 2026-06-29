// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v0/parser.hpp"
#include "detail/v1/parser.hpp"

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/parse.hpp>

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <optional>
#include <vector>

namespace mechanism_configuration
{
  // The detected version plus the source location of the `version` field, when the document
  // had one (a directory or version-less document has no location).
  struct DetectedVersion
  {
    Version version;
    std::optional<ErrorLocation> location;
  };

  std::expected<DetectedVersion, Errors> GetVersion(const std::filesystem::path& config_path)
  {
    if (!std::filesystem::exists(config_path))
    {
      return std::unexpected(Errors{
          { ErrorCode::FileNotFound, mc_fmt::format("Configuration file '{}' does not exist.", config_path.string()) } });
    }

    if (std::filesystem::exists(config_path) && std::filesystem::is_directory(config_path))
    {
      return DetectedVersion{ Version(0, 0, 0), std::nullopt };
    }

    YAML::Node object;
    try
    {
      object = YAML::LoadFile(config_path.string());
    }
    catch (const YAML::Exception& e)
    {
      return std::unexpected(Errors{
          { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse '{}': {}", config_path.string(), e.what()) } });
    }

    // The version field is version-agnostic (it's how we dispatch), so read it literally
    // rather than depending on any version's key vocabulary.
    if (!object["version"])
    {
      // assume it's a v0 config
      return DetectedVersion{ Version(0, 0, 0), std::nullopt };
    }

    const YAML::Node version_node = object["version"];
    return DetectedVersion{ Version(version_node.as<std::string>()),
                            ErrorLocation{ version_node.Mark().line, version_node.Mark().column } };
  }

  std::expected<Mechanism, Errors> Parse(const std::filesystem::path& config_path)
  {
    auto version = GetVersion(config_path);

    if (!version)
    {
      return std::unexpected(std::move(version.error()));
    }

    switch (version->version.major)
    {
      case 0: return v0::Parser{}.Parse(config_path);
      case 1: return v1::Parser{}.Parse(config_path);
      default:
      {
        // We only reach here after reading the version out of config_path, so it names a real
        // file and the version field has a location; point the error at it (path:line:col).
        const std::string body =
            version->location
                ? mc_fmt::format(
                      "{} error: Unsupported version number '{}'.", *version->location, version->version.to_string())
                : mc_fmt::format("error: Unsupported version number '{}'.", version->version.to_string());
        return std::unexpected(Errors{ { ErrorCode::InvalidVersion, config_path.string() + ":" + body } });
      }
    }
  }

  std::expected<Mechanism, Errors> ParseFromString(const std::string& config)
  {
    // only v1 supports parsing from a string, so we must first detect the version from the string
    YAML::Node object;
    try
    {
      object = YAML::Load(config);
      if (!object["version"])
      {
        return std::unexpected(Errors{ { ErrorCode::InvalidVersion, "error: Unsupported version number '0.0.0'." } });
      }
      const Version version(object["version"].as<std::string>());
      if (version.major != 1)
      {
        return std::unexpected(Errors{ { ErrorCode::InvalidVersion, mc_fmt::format("error: Unsupported version number '{}'.", version.to_string()) } });
      }
      return v1::Parser{}.Parse(config);
    }
    catch (const YAML::Exception& e)
    {
      return std::unexpected(Errors{ { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse configuration string: {}", e.what()) } });
    }
  }

}  // namespace mechanism_configuration

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/type_parsers.hpp>
#include <mechanism_configuration/v1/type_validators.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/validate_schema.hpp>
#include <mechanism_configuration/validation.hpp>

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <string_view>

namespace mechanism_configuration
{
  namespace v1
  {
    namespace
    {
      // How a top-level section (species / phases / reactions) is expressed.
      enum class EntityFormat
      {
        FileList,  // { "files": [...] } — split across other files (v1.1+)
        Inline,    // [ ... ] — listed directly
        Invalid,   // present but neither of the above
      };

      EntityFormat GetEntityFormat(const YAML::Node& node)
      {
        if (node.IsMap() && node["files"])
          return EntityFormat::FileList;
        if (node.IsSequence())
          return EntityFormat::Inline;
        return EntityFormat::Invalid;
      }
    }  // namespace

    YAML::Node Parser::FileToYaml(const std::filesystem::path& config_path)
    {
      if (!std::filesystem::exists(config_path) || !std::filesystem::is_regular_file(config_path))
      {
        throw std::runtime_error(
            mc_fmt::format("Configuration file '{}' does not exist or is not a regular file.", config_path.string()));
      }

      SetConfigPath(config_path.string());

      try
      {
        return YAML::LoadFile(config_path.string());
      }
      catch (const YAML::Exception& e)
      {
        throw std::runtime_error(mc_fmt::format("Failed to parse '{}': {}", config_path.string(), e.what()));
      }
    }

    std::expected<YAML::Node, Errors> Parser::ResolveFileConfig(const std::filesystem::path& config_path)
    {
      if (!std::filesystem::exists(config_path) || !std::filesystem::is_regular_file(config_path))
      {
        return std::unexpected(Errors{ { ErrorCode::FileNotFound,
                                         mc_fmt::format("Configuration file '{}' does not exist or is not a regular file.",
                                                        config_path.string()) } });
      }

      SetConfigPath(config_path.string());

      YAML::Node object;
      try
      {
        object = YAML::LoadFile(config_path.string());
      }
      catch (const std::exception& e)
      {
        return std::unexpected(
            Errors{ { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse '{}': {}", config_path.string(), e.what()) } });
      }

      Errors errors;
      const std::filesystem::path base_dir = config_path.parent_path();
      const Version version =
          object[validation::version] ? Version(object[validation::version].as<std::string>()) : Version();

      YAML::Node combined;
      if (object[validation::version])
        combined[std::string(validation::version)] = object[validation::version];
      if (object[validation::name])
        combined[std::string(validation::name)] = object[validation::name];

      bool any_filelist = false;

      // Loads and concatenates every file referenced under `<entity>.files`.
      auto load_files = [&](std::string_view entity) -> YAML::Node
      {
        YAML::Node merged(YAML::NodeType::Sequence);
        for (const auto& file_node : object[std::string(entity)]["files"])
        {
          const std::filesystem::path file_path = base_dir / file_node.as<std::string>();
          if (!std::filesystem::exists(file_path))
          {
            errors.push_back({ ErrorCode::FileNotFound, "File not found: " + file_path.string() });
            continue;
          }
          try
          {
            YAML::Node loaded = YAML::LoadFile(file_path.string());
            for (const auto& item : loaded)
              merged.push_back(item);
          }
          catch (const std::exception& e)
          {
            errors.push_back({ ErrorCode::UnexpectedError, "Failed to parse file: " + file_path.string() + ": " + e.what() });
          }
        }
        return merged;
      };

      // Resolves one section into the combined node. Missing sections are left out so the
      // normal schema validation reports them; malformed sections are flagged here.
      auto resolve_section = [&](std::string_view entity)
      {
        const std::string key(entity);
        if (!object[key])
          return;

        switch (GetEntityFormat(object[key]))
        {
          case EntityFormat::Inline:
            combined[key] = object[key];
            break;
          case EntityFormat::FileList:
            any_filelist = true;
            combined[key] = load_files(entity);
            break;
          case EntityFormat::Invalid:
            if (object[key].IsMap())
              errors.push_back({ ErrorCode::RequiredKeyNotFound, "Missing 'files' key in '" + key + "' section." });
            else
              errors.push_back({ ErrorCode::InvalidType, "'" + key + "' must be a file-list object or an inline array." });
            break;
        }
      };
      resolve_section(validation::species);
      resolve_section(validation::phases);
      resolve_section(validation::reactions);

      if (any_filelist && version.minor < 1)
      {
        errors.push_back({ ErrorCode::InvalidVersion,
                           "File-list format requires minor version >= 1, got " + std::to_string(version.minor) + "." });
      }

      if (!errors.empty())
      {
        AppendFilePath(config_path_, errors);
        return std::unexpected(std::move(errors));
      }

      return combined;
    }

    Errors Parser::Validate(const YAML::Node& object, bool read_from_config_file)
    {
      if (!read_from_config_file)
      {
        SetDefaultConfigPath();
      }

      Errors errors;

      std::vector<std::string_view> required_keys = {
        validation::version, validation::species, validation::phases, validation::reactions
      };
      std::vector<std::string_view> optional_keys = { validation::name };

      // Return early if the required keys are not found
      auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
      if (!validation_errors.empty())
      {
        AppendFilePath(config_path_, validation_errors);
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        return errors;
      }

      constexpr unsigned int MAJOR_VERSION = 1;
      Version version = Version(object[validation::version].as<std::string>());
      if (version.major != MAJOR_VERSION)
      {
        ErrorLocation error_location{ object[validation::version].Mark().line, object[validation::version].Mark().column };

        std::string message = mc_fmt::format(
            "{} error: The version must be '{}' but the invalid version number '{}' found.",
            error_location,
            MAJOR_VERSION,
            version.major);
        errors.push_back({ ErrorCode::InvalidVersion, config_path_ + ":" + message });
      }

      validation_errors = ValidateSpecies(object[validation::species]);
      if (!validation_errors.empty())
      {
        AppendFilePath(config_path_, validation_errors);
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        return errors;
      }

      auto parsed_species = ParseSpecies(object[validation::species]);

      validation_errors = ValidatePhases(object[validation::phases], parsed_species);
      if (!validation_errors.empty())
      {
        AppendFilePath(config_path_, validation_errors);
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        return errors;
      }

      auto parsed_phases = ParsePhases(object[validation::phases]);

      validation_errors = ValidateReactions(object[validation::reactions], parsed_species, parsed_phases);
      if (!validation_errors.empty())
      {
        AppendFilePath(config_path_, validation_errors);
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
      }

      return errors;
    }

    Mechanism Parser::Parse(const YAML::Node& object)
    {
      Mechanism mechanism;

      mechanism.version = Version(object[validation::version].as<std::string>());
      mechanism.species = ParseSpecies(object[validation::species]);
      mechanism.phases = ParsePhases(object[validation::phases]);
      mechanism.reactions = ParseReactions(object[validation::reactions]);

      if (object[validation::name])
      {
        mechanism.name = object[validation::name].as<std::string>();
      }

      return mechanism;
    }

  }  // namespace v1
}  // namespace mechanism_configuration

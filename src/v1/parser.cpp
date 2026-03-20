// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/v1/mechanism_parsers.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/v1/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration
{
  namespace v1
  {
    Parser::EntityFormat Parser::GetEntityFormat(const YAML::Node& node)
    {
      if (node.IsMap() && node["files"])
        return EntityFormat::FileList;
      if (node.IsSequence())
        return EntityFormat::Inline;
      return EntityFormat::Invalid;
    }

    ParserResult<types::Mechanism> Parser::ParseFromString(const std::string& content)
    {
      ParserResult<types::Mechanism> result;
      try
      {
        YAML::Node object = YAML::Load(content);
        return ParseFromNode(object);
      }
      catch (const std::exception& e)
      {
        std::string msg = "Failed to parse content as YAML: " + std::string(e.what());
        msg += "\nContent:\n" + content;
        result.errors.push_back({ ConfigParseStatus::UnexpectedError, msg });
      }

      return result;
    }

    ParserResult<types::Mechanism> Parser::Parse(const std::filesystem::path& config_path)
    {
      ParserResult<types::Mechanism> result;

      if (!std::filesystem::exists(config_path) || !std::filesystem::is_regular_file(config_path))
      {
        result.errors.push_back({ ConfigParseStatus::FileNotFound, "File not found or is a directory" });
        return result;
      }

      try
      {
        YAML::Node object = YAML::LoadFile(config_path.string());

        EntityFormat spc_format = GetEntityFormat(object[validation::species]);
        EntityFormat phs_format = GetEntityFormat(object[validation::phases]);
        EntityFormat rxn_format = GetEntityFormat(object[validation::reactions]);

        if (spc_format == EntityFormat::Inline && 
            phs_format == EntityFormat::Inline && 
            rxn_format == EntityFormat::Inline)
        {
          result = ParseFromNode(object);
        }
        else if (spc_format == EntityFormat::FileList && 
                 phs_format == EntityFormat::FileList && 
                 rxn_format == EntityFormat::FileList)
        {
          result = ParseFromFileConfig(object, config_path);
        }
        else
        {
          auto format_name = [](EntityFormat f) -> std::string {
            return f == EntityFormat::FileList ? "file-list" : "inline";
          };
          std::string msg = "Mixed entity formats are not allowed. "
                            "All sections must use the same format (file-list or inline). "
                            "Got: species=" + format_name(spc_format) +
                            ", phases=" + format_name(phs_format) +
                            ", reactions=" + format_name(rxn_format);
          result.errors.push_back({ ConfigParseStatus::InvalidType, msg });
        }
      }
      catch (const std::exception& e)
      {
        std::string msg = "Failed to parse '" + config_path.string() + "': " + std::string(e.what());
        result.errors.push_back({ ConfigParseStatus::UnexpectedError, msg });
        return result;
      }

      if (!result.errors.empty())
        return result;

      const std::string prefix = config_path.string() + ":";
      for (auto& error : result.errors)
      {
        error.second = prefix + error.second;
      }

      return result;
    }

    ParserResult<types::Mechanism> Parser::ParseFromNode(const YAML::Node& object)
    {
      ParserResult<types::Mechanism> result;
      std::unique_ptr<types::Mechanism> mechanism = std::make_unique<types::Mechanism>();

      std::vector<std::string> mechanism_required_keys = {
        validation::version, validation::species, validation::phases, validation::reactions
      };
      std::vector<std::string> mechanism_optional_keys = { validation::name };

      auto validate = ValidateSchema(object, mechanism_required_keys, mechanism_optional_keys);

      if (!validate.empty())
      {
        result.errors = validate;
        return result;
      }

      Version version = Version(object[validation::version].as<std::string>());

      if (version.major != 1)
      {
        result.errors.push_back({ ConfigParseStatus::InvalidVersion, "Invalid version." });
        return result;
      }

      mechanism->version = version;

      std::string name = object[validation::name].as<std::string>();
      mechanism->name = name;

      auto species_parsing = ParseSpecies(object[validation::species]);
      result.errors.insert(result.errors.end(), species_parsing.first.begin(), species_parsing.first.end());
      mechanism->species = species_parsing.second;

      auto phases_parsing = ParsePhases(object[validation::phases], species_parsing.second);
      result.errors.insert(result.errors.end(), phases_parsing.first.begin(), phases_parsing.first.end());
      mechanism->phases = phases_parsing.second;

      auto reactions_parsing = ParseReactions(object[validation::reactions], species_parsing.second, phases_parsing.second);
      result.errors.insert(result.errors.end(), reactions_parsing.first.begin(), reactions_parsing.first.end());
      mechanism->reactions = reactions_parsing.second;

      result.mechanism = std::move(mechanism);
      return result;
    }

    ParserResult<types::Mechanism> Parser::ParseFromFileConfig(
      const YAML::Node& object, 
      const std::filesystem::path& config_path)
    {
      ParserResult<types::Mechanism> result;

      std::filesystem::path base_dir = config_path.parent_path();

      auto load_files = [&](const std::string& entity) -> std::pair<Errors, YAML::Node>
      {
        Errors errors;
        YAML::Node merged = YAML::Node(YAML::NodeType::Sequence);

        if (!object[entity] || !object[entity]["files"])
        {
          errors.push_back({ ConfigParseStatus::RequiredKeyNotFound,
                            "Missing '" + entity + ".files' in config" });
          return { errors, merged };
        }

        for (const auto& file_node : object[entity]["files"])
        {
          std::filesystem::path file_path = base_dir / file_node.as<std::string>();
          if (!std::filesystem::exists(file_path))
          {
            errors.push_back({ ConfigParseStatus::FileNotFound,
                              "File not found: " + file_path.string() });
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
            errors.push_back({ ConfigParseStatus::UnexpectedError,
                              "Failed to parse file: " + file_path.string() + ": " + e.what() });
          }
        }
        return { errors, merged };
      };

      YAML::Node combined;
      combined[validation::version] = object[validation::version];
      combined[validation::name] = object[validation::name];

      auto [species_errors, species_node] = load_files(validation::species);
      result.errors.insert(result.errors.end(), species_errors.begin(), species_errors.end());
      combined[validation::species] = species_node;

      auto [phases_errors, phases_node] = load_files(validation::phases);
      result.errors.insert(result.errors.end(), phases_errors.begin(), phases_errors.end());
      combined[validation::phases] = phases_node;

      auto [reactions_errors, reactions_node] = load_files(validation::reactions);
      result.errors.insert(result.errors.end(), reactions_errors.begin(), reactions_errors.end());
      combined[validation::reactions] = reactions_node;

      return ParseFromNode(combined);
    }
  }  // namespace v1
}  // namespace mechanism_configuration
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

        auto parsed = ParseFromNode(object);
        // prepend the file name to the error messages
        for (auto& error : parsed.errors)
        {
          error.second = config_path.string() + ":" + error.second;
        }

        return parsed;
      }
      catch (const std::exception& e)
      {
        std::string msg = "Failed to parse file as YAML: " + std::string(e.what());
        msg += "\nFile: " + config_path.string();
        result.errors.push_back({ ConfigParseStatus::UnexpectedError, msg });
        return result;
      }
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
  }  // namespace v1
}  // namespace mechanism_configuration
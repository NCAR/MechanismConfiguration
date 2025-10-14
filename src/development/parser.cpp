// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/mechanism_parsers.hpp>
#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/utils.hpp>
#include <mechanism_configuration/development/validation.hpp>
#include <mechanism_configuration/development/validator.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration
{
  namespace development
  {
    ParserResult<types::Mechanism> Parser::Parse(const std::filesystem::path& config_path)
    {
      ParserResult<types::Mechanism> result;
      if (!std::filesystem::exists(config_path) || !std::filesystem::is_regular_file(config_path))
      {
        result.errors.push_back({ ConfigParseStatus::FileNotFound, "File not found or is a directory" });
        return result;
      }
      YAML::Node object = YAML::LoadFile(config_path.string());
      std::unique_ptr<types::Mechanism> mechanism = std::make_unique<types::Mechanism>();

      std::vector<std::string> mechanism_required_keys = { validation::version, validation::species, validation::phases, validation::reactions };
      std::vector<std::string> mechanism_optional_keys = { validation::name, validation::models };

      auto validate = ValidateSchema(object, mechanism_required_keys, mechanism_optional_keys);

      if (!validate.empty())
      {
        result.errors = validate;
        return result;
      }

      Version version = Version(object[validation::version].as<std::string>());

      if (version.major != 2)
      {
        result.errors.push_back({ ConfigParseStatus::InvalidVersion, "Invalid version." });
        return result;
      }

      mechanism->version = version;

      std::string name = object[validation::name].as<std::string>();
      mechanism->name = name;

      // Species
      auto validation_errors = ValidateSpecies(object[validation::species]);
      if (!validation_errors.empty())
      {
        result.errors.insert(result.errors.end(), validation_errors.begin(), validation_errors.end());
        return result;
      }

      auto parsed_species = ParseSpecies(object[validation::species]);
      mechanism->species = parsed_species;

      // Phases
      validation_errors = ValidatePhases(object[validation::phases], parsed_species);
      if (!validation_errors.empty())
      {
        AppendFilePath(config_path, validation_errors);
        result.errors.insert(result.errors.end(), validation_errors.begin(), validation_errors.end());
        return result;
      }

      auto parsed_phases = ParsePhases(object[validation::phases]);
      mechanism->phases = parsed_phases;

      // Models
      YAML::Node models_node = object[validation::models];
      if (models_node && !models_node.IsNull())
      {
        auto models_parsing = ParseModels(models_node, parsed_phases);
        result.errors.insert(result.errors.end(), models_parsing.first.begin(), models_parsing.first.end());
        mechanism->models = models_parsing.second;
      }

      // Reactions
      validation_errors = ValidateReactions(object[validation::reactions], parsed_species, parsed_phases);
      if (!validation_errors.empty())
      {
        AppendFilePath(config_path, validation_errors);
        result.errors.insert(result.errors.end(), validation_errors.begin(), validation_errors.end());
        return result;
      }

      // auto parsed_reactions = ParseReactions(object[validation::reactions]);
      auto parsed_reactions = ParseReactions(object[validation::reactions], parsed_species, parsed_phases);
      result.errors.insert(result.errors.end(), parsed_reactions.first.begin(), parsed_reactions.first.end());
      mechanism->reactions = parsed_reactions.second;

      result.mechanism = std::move(mechanism);


      return result;
    }
  }  // namespace development
}  // namespace mechanism_configuration

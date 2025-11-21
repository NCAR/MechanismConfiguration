// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/type_parsers.hpp>
#include <mechanism_configuration/development/mechanism.hpp>
#include <mechanism_configuration/development/utils.hpp>
#include <mechanism_configuration/development/validation.hpp>
#include <mechanism_configuration/development/type_validators.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration
{
  namespace development
  {

    YAML::Node Parser::FileToYaml(const std::filesystem::path& config_path)
    {
      if (!std::filesystem::exists(config_path) || !std::filesystem::is_regular_file(config_path))
      {
        throw std::runtime_error(std::format(
          "Configuration file '{}' does not exist or is not a regular file.", config_path.string()));
      }

      SetConfigPath(config_path.string());

      try
      {
        return YAML::LoadFile(config_path.string());
      } 
      catch (const YAML::ParserException& e)
      {
        throw std::runtime_error(std::format(
              "Failed to parse '{}': {}", config_path.string(), e.what()));
      }
    }

    Errors Parser::Validate(const YAML::Node& object, bool read_from_config_file)
    {
      if (!read_from_config_file)
      {
        SetDefaultConfigPath();
      }

      Errors errors;

      std::vector<std::string> required_keys = {
        validation::version, validation::species, validation::phases, validation::reactions
      };
      std::vector<std::string> optional_keys = { validation::name, validation::models };

      // Return early if the required keys are not found
      auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
      if (!validation_errors.empty())
      {
        AppendFilePath(config_path_, validation_errors);
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        return errors;
      }

      constexpr unsigned int MAJOR_VERSION = 2;
      Version version = Version(object[validation::version].as<std::string>());
      if (version.major != MAJOR_VERSION)
      {
        std::string message = std::format(
          "The version must be '{}' but the invalid verison number '{}' found.", MAJOR_VERSION, version.major);
        errors.push_back({ ConfigParseStatus::InvalidVersion, config_path_ + ":" + message });
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

      if (object[validation::models])
      {
        validation_errors = ValidateModels(object[validation::models], parsed_phases);
        if (!validation_errors.empty())
        {
          AppendFilePath(config_path_, validation_errors);
          errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        }
      }

      return errors;
    }

    types::Mechanism Parser::Parse(const YAML::Node& object)
    {
      types::Mechanism mechanism;

      mechanism.version = Version(object[validation::version].as<std::string>());
      mechanism.species = ParseSpecies(object[validation::species]);
      mechanism.phases = ParsePhases(object[validation::phases]);
      mechanism.reactions = ParseReactions(object[validation::reactions]);

      if (object[validation::name])
      {
        mechanism.name = object[validation::name].as<std::string>();
      }
      if (object[validation::models])
      {
        mechanism.models = ParseModels(object[validation::models]);
      }

      return mechanism;
    }

  }  // namespace development
}  // namespace mechanism_configuration

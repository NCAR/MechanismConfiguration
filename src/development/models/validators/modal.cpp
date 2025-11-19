// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/model_parsers.hpp>
#include <mechanism_configuration/development/model_types.hpp>
#include <mechanism_configuration/development/utils.hpp>
#include <mechanism_configuration/development/validator.hpp>
#include <mechanism_configuration/error_location.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <utility>

namespace mechanism_configuration
{
  namespace development
  {
    Errors ModalModelParser::Validate(const YAML::Node& object, const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string> required_top_level_keys = { validation::type, validation::modes };
      std::vector<std::string> optional_top_level_keys = { validation::name };

      std::vector<std::string> required_second_level_keys = {
        validation::name, validation::geometric_mean_diameter, validation::geometric_standard_deviation, validation::phase
      };
      std::vector<std::string> optional_second_level_keys = {};

      Errors errors;

      auto validation_errors = ValidateSchema(object, required_top_level_keys, optional_top_level_keys);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        return errors;
      }

      if (!object[validation::modes].IsSequence())
      {
        const auto& node = object[validation::modes];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string message = std::format(
            "{} error: Expected 'modes' to be a sequence, but found a different type in the '{}' model.",
            error_location,
            object[validation::type].as<std::string>());

        errors.push_back({ ConfigParseStatus::InvalidType, message });

        return errors;
      }

      for (const auto& mode_object : object[validation::modes])
      {
        validation_errors = ValidateSchema(mode_object, required_second_level_keys, optional_second_level_keys);
        if (!validation_errors.empty())
        {
          errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
          continue;
        }

        auto phase_optional = CheckPhaseExists(
            mode_object,
            validation::phase,
            existing_phases,
            errors,
            ConfigParseStatus::UnknownPhase,
            object[validation::type].as<std::string>());
      }

      return errors;
    }

  }  // namespace development
}  // namespace mechanism_configuration

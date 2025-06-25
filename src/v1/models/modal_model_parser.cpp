// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/v1/model_parsers.hpp>
#include <mechanism_configuration/v1/model_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include  <utility>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors ModalModelParser::parse(
        const YAML::Node& object,
        const std::vector<types::Phase>& existing_phases,
        types::Models& models)
    {
      Errors errors;
      types::ModalModel model;

      std::vector<std::string> required_top_level_keys = { validation::type, validation::modes };
      std::vector<std::string> optional_top_level_keys = { validation::name };

      std::vector<std::string> required_second_level_keys = { validation::name, 
        validation::geometric_mean_diameter, validation::geometric_standard_deviation, validation::phase };
      std::vector<std::string> optional_second_level_keys = { };

      auto has_error_top_level_keys = ValidateSchema(object, required_top_level_keys , optional_top_level_keys);
      errors.insert(errors.end(), has_error_top_level_keys.begin(), has_error_top_level_keys.end());

      // Check the top level keys are valid
      if (!has_error_top_level_keys.empty())
      {
        models.modal_model = std::move(model);
        return errors;
      }

      // Check the keys in each mode are valid
      bool no_errors_in_second_level_keys = true;
      for (const auto& mode_object : object[validation::modes])
      {
        auto has_error_second_level_keys = ValidateSchema(mode_object, 
            required_second_level_keys , optional_second_level_keys);
        errors.insert(errors.end(), has_error_second_level_keys.begin(), has_error_second_level_keys.end());

        if (!has_error_second_level_keys.empty())
        {
          no_errors_in_second_level_keys = false;
        }
      }

      if (no_errors_in_second_level_keys)
      {
        model.type = object[validation::type].as<std::string>();

        if (object[validation::name])
        {
          model.name = object[validation::name].as<std::string>();
        }
        
        for (const auto& mode_object : object[validation::modes])
        {
          types::Mode mode;
          mode.name = mode_object[validation::name].as<std::string>();
          mode.geometric_mean_diameter = mode_object[validation::geometric_mean_diameter].as<double>();
          mode.geometric_standard_deviation = mode_object[validation::geometric_standard_deviation].as<double>();

          // Check whether the phase for the mode is valid by comparing it to the initialized phases
          std::string mode_phase = mode_object[validation::phase].as<std::string>();
          auto it_found_phase =
              std::find_if(existing_phases.begin(), existing_phases.end(), [&mode_phase](const auto& phase) { return phase.name == mode_phase; });
          if (it_found_phase == existing_phases.end())
          {
            std::string line = std::to_string(mode_object[validation::phase].Mark().line + 1);
            std::string column = std::to_string(mode_object[validation::phase].Mark().column + 1);
            errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + mode_phase });
          }
          else
          {
            mode.phase = mode_phase;
          }

          mode.unknown_properties = GetComments(mode_object);
          model.modes.push_back(mode);
        }

        model.unknown_properties = GetComments(object);
        models.modal_model = std::move(model);
      }

      return errors;
    }

  }  // namespace v1
}  // namespace mechanism_configuration

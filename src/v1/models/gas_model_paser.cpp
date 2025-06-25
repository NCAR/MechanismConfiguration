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
    Errors GasModelParser::parse(
        const YAML::Node& object,
        const std::vector<types::Phase>& existing_phases,
        types::Models& models)
    {
      Errors errors;
      types::GasModel model;

      std::vector<std::string> required_keys = { validation::type, validation::phase };
      std::vector<std::string> optional_keys = { validation::name };

      auto has_error = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), has_error.begin(), has_error.end());
      if (!has_error.empty())
      {
        models.gas_model = std::move(model);
      }
      else
      {
        model.type = object[validation::type].as<std::string>();

        // Check whether the phase for the model is valid by comparing it to the initialized phases
        std::string model_phase = object[validation::phase].as<std::string>();
        auto it_found_phase =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&model_phase](const auto& phase) { return phase.name == model_phase; });
        if (it_found_phase == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + model_phase });
        }
        else
        {
          model.phase = model_phase;
        }

        if (object[validation::name])
        {
          model.name = object[validation::name].as<std::string>();
        }

        model.unknown_properties = GetComments(object);
        models.gas_model = std::move(model);
      }

      return errors;
    }

  }  // namespace v1
}  // namespace mechanism_configuration

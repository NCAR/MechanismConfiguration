// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/model_parsers.hpp>
#include <mechanism_configuration/development/model_types.hpp>
#include <mechanism_configuration/development/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <utility>

namespace mechanism_configuration
{
  namespace development
  {
    Errors GasModelParser::Validate(const YAML::Node& object, const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string> required_keys = { validation::type, validation::phase };
      std::vector<std::string> optional_keys = { validation::name };

      Errors errors;

      auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        return errors;
      }

      auto phase_optional = CheckPhaseExists(object, validation::phase, existing_phases, errors);

      return errors;
    }

  }  // namespace development
}  // namespace mechanism_configuration

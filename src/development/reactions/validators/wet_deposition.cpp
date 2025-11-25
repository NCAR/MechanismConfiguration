// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/reaction_parsers.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/type_parsers.hpp>
#include <mechanism_configuration/development/type_validators.hpp>
#include <mechanism_configuration/development/utils.hpp>
#include <mechanism_configuration/error_location.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <format>

namespace mechanism_configuration
{
  namespace development
  {
    /// @brief Validates a YAML-defined Wet Deposition reaction entry.
    ///        Performs schema validation and ensures the referenced phase exists.
    /// @param object The YAML node representing the reaction
    /// @param existing_species The list of known species used for validation, not used
    /// @param existing_phases The list of known phases used for validation
    /// @return A list of validation errors, if any
    Errors WetDepositionParser::Validate(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string> required_keys = { validation::condensed_phase, validation::type };
      std::vector<std::string> optional_keys = { validation::name, validation::scaling_factor };

      Errors errors;

      auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        return errors;
      }

      // Check for phase existence
      auto phase_optional = CheckPhaseExists(object, validation::condensed_phase, existing_phases, errors);
      if (!phase_optional)
      {
        return errors;
      }

      return errors;
    }

  }  // namespace development
}  // namespace mechanism_configuration
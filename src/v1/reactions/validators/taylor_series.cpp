// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/v1/reaction_parsers.hpp>
#include <mechanism_configuration/types.hpp>
#include <mechanism_configuration/v1/type_parsers.hpp>
#include <mechanism_configuration/v1/type_validators.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    /// @brief Validates a YAML-defined Taylor Series reaction entry
    ///        Performs schema validation, checks for mutually exclusive parameters (`Ea` vs `C`),
    ///        ensures all referenced species and phases exist, and collects any errors found.
    /// @param object The YAML node representing the reaction
    /// @param existing_species The list of known species used for validation
    /// @param existing_phases The list of known phases used for validation
    /// @return A list of validation errors, if any
    Errors TaylorSeriesParser::Validate(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string_view> required_keys = {
        validation::reactants, validation::products, validation::type, validation::gas_phase
      };
      std::vector<std::string_view> optional_keys = { validation::A,    validation::B,
                                                 validation::C,    validation::D,
                                                 validation::E,    validation::Ea,
                                                 validation::name, validation::taylor_coefficients };
      Errors errors;

      auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        return errors;
      }

      bool is_valid = true;

      // Reactants
      validation_errors = ValidateReactantsOrProducts(object[validation::reactants]);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        is_valid = false;
      }

      // Products
      validation_errors = ValidateReactantsOrProducts(object[validation::products]);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        is_valid = false;
      }

      if (object[validation::Ea] && object[validation::C])
      {
        const auto& node = object[validation::Ea];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string message = mc_fmt::format(
            "{} error: Mutually exclusive option of 'Ea' and 'C' found in '{}' reaction.",
            error_location,
            object[validation::type].as<std::string>());

        errors.push_back({ ErrorCode::MutuallyExclusiveOption, message });
        is_valid = false;
      }

      if (!is_valid)
        return errors;

      // Semantic checks are performed by the version-neutral ValidateSemantics.

      return errors;
    }

  }  // namespace v1
}  // namespace mechanism_configuration

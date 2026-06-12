// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <detail/v1/reaction_parsers.hpp>
#include <mechanism_configuration/types.hpp>
#include <detail/v1/type_parsers.hpp>
#include <detail/v1/type_validators.hpp>
#include <detail/v1/utils.hpp>
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <detail/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    /// @brief Validates a YAML-defined Surface reaction entry.
    ///        Performs schema validation, ensures all referenced species and phases exist,
    ///        and collects any errors found.
    /// @param object The YAML node representing the reaction
    /// @param existing_species The list of known species used for validation
    /// @param existing_phases The list of known phases used for validation
    /// @return A list of validation errors, if any
    Errors SurfaceParser::Validate(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string_view> required_keys = { validation::gas_phase_products,
                                                 validation::gas_phase_species,
                                                 validation::type,
                                                 validation::gas_phase };
      std::vector<std::string_view> optional_keys = { validation::name,
                                                 validation::reaction_probability,
                                                 validation::condensed_phase };

      Errors errors;

      auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        return errors;
      }

      bool is_valid = true;

      // Gas phase species reactant
      validation_errors = ValidateReactantsOrProducts(object[validation::gas_phase_species]);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        is_valid = false;
      }

      // Products
      validation_errors = ValidateReactantsOrProducts(object[validation::gas_phase_products]);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        is_valid = false;
      }

      if (!is_valid)
        return errors;

      // The gas-phase species (reactant) must belong to the reaction's phase; gas-phase
      // products may reference any phase, so only the reactant is phase-checked below.
      std::vector<std::pair<types::ReactionComponent, YAML::Node>> reactant_node_pairs;
      for (const auto& obj : object[validation::gas_phase_species])
      {
        types::ReactionComponent component;
        component.name = GetReactionComponentName(obj);
        reactant_node_pairs.emplace_back(component, obj);
      }

      // Validates the number of reactants
      // This must be done before collecting errors from the products
      if (reactant_node_pairs.size() > 1)
      {
        const auto& node = object[validation::gas_phase_species];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string message = mc_fmt::format(
            "{} error: '{}' reaction requires one reactant, but {} were provided.",
            error_location,
            object[validation::type].as<std::string>(),
            reactant_node_pairs.size());

        errors.push_back({ ErrorCode::TooManyReactionComponents, message });
      }

      // Semantic checks (species existence, phase membership) are performed by the
      // version-neutral ValidateSemantics over the canonical Mechanism.
      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

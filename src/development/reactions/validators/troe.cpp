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

namespace mechanism_configuration
{
  namespace development
  {
    /// @brief Validates a YAML-defined Troe reaction entry
    ///        Performs schema validation, ensures all referenced species and phases exist,
    ///        and collects any errors found.
    /// @param object The YAML node representing the reaction
    /// @param existing_species The list of known species used for validation
    /// @param existing_phases The list of known phases used for validation
    /// @return A list of validation errors, if any
    Errors TroeParser::Validate(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string> required_keys = {
        validation::reactants, validation::products, validation::type, validation::gas_phase
      };
      std::vector<std::string> optional_keys = { validation::name,   validation::k0_A,   validation::k0_B,
                                                 validation::k0_C,   validation::kinf_A, validation::kinf_B,
                                                 validation::kinf_C, validation::Fc,     validation::N };
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

      if (!is_valid)
        return errors;

      std::vector<std::pair<types::ReactionComponent, YAML::Node>> species_node_pairs;

      for (const auto& obj : object[validation::reactants])
      {
        types::ReactionComponent component;
        component.name = obj[validation::name].as<std::string>();
        species_node_pairs.emplace_back(component, obj);
      }
      for (const auto& obj : object[validation::products])
      {
        types::ReactionComponent component;
        component.name = obj[validation::name].as<std::string>();
        species_node_pairs.emplace_back(component, obj);
      }

      // Check for unknown species in reactants and products
      std::vector<NodeInfo> unknown_species = FindUnknownObjectsByName(existing_species, species_node_pairs);
      if (!unknown_species.empty())
      {
        ReportUnknownSpecies(object, unknown_species, errors, ConfigParseStatus::ReactionRequiresUnknownSpecies);
      }

      // Check for phase existence and get phase reference
      auto phase_optional = CheckPhaseExists(object, validation::gas_phase, existing_phases, errors);
      if (!phase_optional)
      {
        return errors;
      }

      // Check if phase-specific species in reaction is found in phase
      const auto& phase = phase_optional->get();
      CheckSpeciesPresenceInPhase(object, phase, species_node_pairs, errors);

      return errors;
    }

  }  // namespace development
}  // namespace mechanism_configuration

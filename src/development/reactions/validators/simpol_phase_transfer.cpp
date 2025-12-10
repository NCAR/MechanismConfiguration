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
    /// @brief Validates a YAML-defined Simpol Phase Transfer reaction entry.
    ///        Performs schema validation, ensures all referenced species and phases exist,
    ///        and collects any errors found.
    /// @param object The YAML node representing the reaction
    /// @param existing_species The list of known species used for validation
    /// @param existing_phases The list of known phases used for validation
    /// @return A list of validation errors, if any
    Errors SimpolPhaseTransferParser::Validate(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string> required_keys = { validation::type,
                                                 validation::gas_phase,
                                                 validation::gas_phase_species,
                                                 validation::condensed_phase,
                                                 validation::condensed_phase_species,
                                                 validation::B };
      std::vector<std::string> optional_keys = { validation::name };

      Errors errors;

      auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        return errors;
      }

      bool is_valid = true;

      validation_errors = ValidateReactantsOrProducts(object[validation::gas_phase_species]);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        is_valid = false;
      }

      validation_errors = ValidateReactantsOrProducts(object[validation::condensed_phase_species]);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        is_valid = false;
      }

      if (!is_valid)
        return errors;

      // Validate parameter 'B'
      constexpr double NUM_PARAMS = 4;
      if (!object[validation::B].IsSequence() || object[validation::B].size() != NUM_PARAMS)
      {
        const auto& node = object[validation::B];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string issue;
        if (!object[validation::B].IsSequence())
        {
          issue = "value must be a sequence";
        }
        else
        {
          issue = std::format("requires {} parameters, but {} were provided", NUM_PARAMS, object[validation::B].size());
        }

        std::string message = std::format(
            "{} error: '{}' reaction parameter 'B' {}.", error_location, object[validation::type].as<std::string>(), issue);

        errors.push_back({ ConfigParseStatus::InvalidParameterNumber, message });
      }

      // Validates the number of gas phase species
      std::vector<std::pair<types::ReactionComponent, YAML::Node>> gas_species_node_pairs;

      for (const auto& obj : object[validation::gas_phase_species])
      {
        types::ReactionComponent component;
        component.name = obj[validation::name].as<std::string>();
        gas_species_node_pairs.emplace_back(component, obj);
      }

      if (gas_species_node_pairs.size() > 1)
      {
        const auto& node = object[validation::gas_phase_species];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string message = std::format(
            "{} error: '{}' reaction requires one gas phase, but {} were provided.",
            error_location,
            object[validation::type].as<std::string>(),
            gas_species_node_pairs.size());

        errors.push_back({ ConfigParseStatus::TooManyReactionComponents, message });
      }

      // Check for unknown species in gas phase species
      std::vector<NodeInfo> unknown_species = FindUnknownObjectsByName(existing_species, gas_species_node_pairs);
      if (!unknown_species.empty())
      {
        ReportUnknownSpecies(object, unknown_species, errors, ConfigParseStatus::ReactionRequiresUnknownSpecies);
      }

      // Validates the number of condensed phase species
      std::vector<std::pair<types::ReactionComponent, YAML::Node>> condensed_species_node_pairs;

      for (const auto& obj : object[validation::condensed_phase_species])
      {
        types::ReactionComponent component;
        component.name = obj[validation::name].as<std::string>();
        condensed_species_node_pairs.emplace_back(component, obj);
      }

      if (condensed_species_node_pairs.size() > 1)
      {
        const auto& node = object[validation::condensed_phase_species];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string message = std::format(
            "{} error: '{}' reaction requires one condensed phase, but {} were provided.",
            error_location,
            object[validation::type].as<std::string>(),
            condensed_species_node_pairs.size());

        errors.push_back({ ConfigParseStatus::TooManyReactionComponents, message });
      }

      // Check for unknown species in condensed phase species
      unknown_species = FindUnknownObjectsByName(existing_species, condensed_species_node_pairs);
      if (!unknown_species.empty())
      {
        ReportUnknownSpecies(object, unknown_species, errors, ConfigParseStatus::ReactionRequiresUnknownSpecies);
      }

      // Check for phase existence and get phase reference
      auto gas_phase_optional = CheckPhaseExists(object, validation::gas_phase, existing_phases, errors);
      if (!gas_phase_optional)
      {
        return errors;
      }
      auto condensed_phase_optional = CheckPhaseExists(object, validation::condensed_phase, existing_phases, errors);
      if (!condensed_phase_optional)
      {
        return errors;
      }

      // Check if phase-specific species in reaction is found in phase
      const auto& gas_phase = gas_phase_optional->get();
      CheckSpeciesPresenceInPhase(object, gas_phase, gas_species_node_pairs, errors);

      const auto& condensed_phase = condensed_phase_optional->get();
      CheckSpeciesPresenceInPhase(object, condensed_phase, condensed_species_node_pairs, errors);

      return errors;
    }

  }  // namespace development
}  // namespace mechanism_configuration

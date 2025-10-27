// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/mechanism_parsers.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/utils.hpp>
#include <mechanism_configuration/development/validator.hpp>
#include <mechanism_configuration/error_location.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <format>

namespace mechanism_configuration
{
  namespace development
  {
    /// @brief Validates a YAML-defined Henry's Law reaction entry.
    ///        Performs schema validation, ensures all referenced species and phases exist,
    ///        and collects any errors found.
    /// @param object The YAML node representing the reaction
    /// @param existing_species The list of known species used for validation
    /// @param existing_phases The list of known phases used for validation
    /// @return A list of validation errors, if any
    Errors HenrysLawParser::Validate(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string> required_keys = { validation::type, validation::gas, validation::particle };
      std::vector<std::string> opt_keys = { validation::name };

      Errors errors;

      auto validation_errors = ValidateSchema(object, required_keys, opt_keys);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        return errors;
      }

      bool is_valid = true;

      // Gas phase
      validation_errors = ValidatePhases(object[validation::gas], existing_species);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        is_valid = false;
      }

      // Particle
      validation_errors = ValidateParticles(object[validation::particle]);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        is_valid = false;
      }

      if (!is_valid)
        return errors;

      // Gather gas and particle species node pairs
      std::vector<std::pair<types::ReactionComponent, YAML::Node>> gas_species_node_pairs;

      // Although the type being validated is PhaseSpecies instead of ReactionComponent, 
      // the corresponding struct is conceptually the same. (diffusion coefficient vs. coefficient) 
      // Treating these entries as ReactionComponent objects is acceptable.
      for (const auto& obj : object[validation::gas][validation::species])
      {
        types::ReactionComponent component;
        component.name = obj[validation::name].as<std::string>();
        gas_species_node_pairs.emplace_back(std::move(component), obj);
      }

      std::vector<std::pair<types::ReactionComponent, YAML::Node>> particle_species_node_pairs;

      // Solvent
      for (const auto& obj : object[validation::particle][validation::solvent])
      {
        types::ReactionComponent component;
        component.name = obj[validation::name].as<std::string>();
        particle_species_node_pairs.emplace_back(component, obj);
      }

      // Validates the number of solvent
      // This must be done before collecting errors from the solutes
      if (particle_species_node_pairs.size() > 1)
      {
        const auto& node = object[validation::particle][validation::solvent];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string message = std::format(
            "{} error: '{}' reaction requires one solute, but {} were provided.",
            error_location,
            object[validation::type].as<std::string>(),
            particle_species_node_pairs.size());

        errors.push_back({ ConfigParseStatus::TooManyReactionComponents, message });
      }

      // Solutes
      for (const auto& obj : object[validation::particle][validation::solutes])
      {
        types::ReactionComponent component;
        component.name = obj[validation::name].as<std::string>();
        particle_species_node_pairs.emplace_back(std::move(component), obj);
      }

      // Unknown species
      std::vector<NodeInfo> unknown_species = FindUnknownObjectsByName(existing_species, gas_species_node_pairs);
      if (!unknown_species.empty())
      {
        ReportUnknownSpecies(object, unknown_species, errors, ConfigParseStatus::ReactionRequiresUnknownSpecies);
      }

      unknown_species = FindUnknownObjectsByName(existing_species, particle_species_node_pairs);
      if (!unknown_species.empty())
      {
        ReportUnknownSpecies(object, unknown_species, errors, ConfigParseStatus::ReactionRequiresUnknownSpecies);
      }

      auto gas_phase_opt = CheckPhaseExists(object[validation::gas], 
                                            validation::name, 
                                            existing_phases, 
                                            errors, 
                                            ConfigParseStatus::UnknownPhase, 
                                            object[validation::type].as<std::string>());
 
      auto particle_phase_opt = CheckPhaseExists(object[validation::particle], 
                                                validation::phase, 
                                                existing_phases, 
                                                errors, 
                                                ConfigParseStatus::UnknownPhase, 
                                                object[validation::type].as<std::string>());
      if (!gas_phase_opt || !particle_phase_opt)
      {
        return errors;
      }

      // Check if phase-specific species in reaction is found in phase
      CheckSpeciesPresenceInPhase(object, gas_phase_opt->get(), gas_species_node_pairs, errors);
      CheckSpeciesPresenceInPhase(object, particle_phase_opt->get(), particle_species_node_pairs, errors);

      return errors;
    }

  }  // namespace development
}  // namespace mechanism_configuration

// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/development/mechanism_parsers.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/utils.hpp>
#include <mechanism_configuration/validation_errors_schema.hpp>

namespace mechanism_configuration
{
  namespace development
  {
    Errors ArrheniusParser::Validate(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string> required_keys = { validation::products, validation::reactants, 
                                                 validation::type, validation::gas_phase };
      std::vector<std::string> optional_keys = { validation::A, validation::B, validation::C,
                                                 validation::D, validation::E, validation::Ea, 
                                                 validation::name };

      Errors errors;
      bool is_valid = true;

      auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        is_valid = false;
        continue;
      }

      if (!is_valid)
        return errors;
     
      // Reactants
      validation_errors = ValidateReactantsOrProducts(object, validation::reactants);
      if (!validation_errors.empty())
      {
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        is_valid = false;
      }

      // Products
      validation_errors = ValidateReactantsOrProducts(object, validation::products);
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
        component.species_name = obj[validation::species_name].as<std::string>();
        species_node_pairs.emplace_back(component, obj);
      }
      for (const auto& obj : object[validation::products])
      {
        types::ReactionComponent component;
        component.species_name = obj[validation::species_name].as<std::string>();
        species_node_pairs.emplace_back(component, obj);
      }

      // Check for unknown species in reactants and products
      std::vector<NodeInfo> unknown_species = FindUnknownObjectsByName(existing_species, species_node_pairs);
      if (!unknown_species.empty())
      {
        for (const auto& [name, node] : unknown_species)
        {
          ErrorLocation error_location{ node.Mark().line, node.Mark().column };

          std::string message = std::format("{} error: Unknown species name '{}' found in '{}' reaction", 
            error_location, name, object[validation::type].as<std::string>());

          errors.push_back({ ConfigParseStatus::ReactionRequiresUnknownSpecies, message });
        }
      }

      // Check for unknown phase
      const auto& phase_node = object[validation::gas_phase];
      std::string gas_phase = phase_node.as<std::string>();
      auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });

      if (it == existing_phases.end())
      {
        ErrorLocation error_location{ phase_nodee.Mark().line, phase_node.Mark().column };

        std::string message = std::format("{} error: Unknown phase name '{}' found in '{}' reaction", 
          error_location, gas_phase, object[validation::type].as<std::string>());

        errors.push_back({ ConfigParseStatus::UnknownPhase, message });
      }

      return errors;
    }
  }  // namespace development
}  // namespace mechanism_configuration

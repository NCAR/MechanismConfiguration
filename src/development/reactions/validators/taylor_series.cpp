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
      std::vector<std::string> required_keys = {
        validation::reactants, validation::products, validation::type, validation::gas_phase
      };
      std::vector<std::string> optional_keys = { validation::A,    validation::B,
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

      if (object[validation::Ea].IsDefined() && object[validation::C].IsDefined())
      {
        const auto& node = object[validation::Ea];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string message = std::format(
            "{} error: Mutually exclusive option of 'Ea' and 'C' found in '{}' reaction.",
            error_location,
            object[validation::type].as<std::string>());

        errors.push_back({ ConfigParseStatus::MutuallyExclusiveOption, message });
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
      ReportUnknownSpecies(object, unknown_species, errors, ConfigParseStatus::ReactionRequiresUnknownSpecies);

      // Check for unknown phase
      CheckPhaseExists(object, validation::gas_phase, existing_phases, errors);

      return errors;
    }

  }  // namespace development
}  // namespace mechanism_configuration

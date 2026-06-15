// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <detail/v1/reaction_parsers.hpp>
#include <mechanism_configuration/types.hpp>
#include <detail/v1/type_parsers.hpp>
#include <detail/v1/type_schema.hpp>
#include <detail/v1/utils.hpp>
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <detail/check_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    /// @brief Checks the structural schema of a YAML-defined Photolysis reaction entry.
    ///        Performs structural (schema) validation only;
    ///        and collects any errors found.
    /// @param object The YAML node representing the reaction
    /// @param existing_species Unused; semantic checks live in ValidateSemantics
    /// @param existing_phases Unused; semantic checks live in ValidateSemantics
    /// @return A list of validation errors, if any
    Errors PhotolysisParser::CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string_view> required_keys = {
        validation::reactants, validation::products, validation::type, validation::gas_phase
      };
      std::vector<std::string_view> optional_keys = { validation::name, validation::scaling_factor };

      Errors errors;

      auto schema_errors = mechanism_configuration::CheckSchema(object, required_keys, optional_keys);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
        return errors;
      }

      // Reactants
      schema_errors = CheckReactantsOrProductsSchema(object[validation::reactants]);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }

      // Products
      schema_errors = CheckReactantsOrProductsSchema(object[validation::products]);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }

      if (!errors.empty())
        return errors;

      // Reactants must belong to the reaction's phase; products may reference any phase.
      std::vector<std::pair<types::ReactionComponent, YAML::Node>> reactant_node_pairs;
      for (const auto& obj : object[validation::reactants])
      {
        types::ReactionComponent component;
        component.name = GetReactionComponentName(obj);
        reactant_node_pairs.emplace_back(component, obj);
      }
      std::vector<std::pair<types::ReactionComponent, YAML::Node>> species_node_pairs = reactant_node_pairs;

      // Checks the number of reactants
      // This must be done before collecting errors from the products
      if (species_node_pairs.size() > 1)
      {
        const auto& node = object[validation::reactants];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string message = mc_fmt::format(
            "{} error: '{}' reaction requires one reactant, but {} were provided.",
            error_location,
            object[validation::type].as<std::string>(),
            species_node_pairs.size());

        errors.push_back({ ErrorCode::TooManyReactionComponents, message });
      }

      // Semantic checks (species existence, phase membership) are performed by the
      // version-neutral ValidateSemantics over the canonical Mechanism.
      return errors;
    }

    void PhotolysisParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Photolysis photolysis;

      photolysis.gas_phase = object[validation::gas_phase].as<std::string>();
      photolysis.reactants = ParseReactionComponent(object, validation::reactants);
      photolysis.products = ParseReactionComponents(object, validation::products);
      photolysis.unknown_properties = GetComments(object);

      if (object[validation::scaling_factor])
      {
        photolysis.scaling_factor = object[validation::scaling_factor].as<double>();
      }
      if (object[validation::name])
      {
        photolysis.name = object[validation::name].as<std::string>();
      }

      reactions.photolysis.emplace_back(std::move(photolysis));
    }

  }  // namespace v1
}  // namespace mechanism_configuration

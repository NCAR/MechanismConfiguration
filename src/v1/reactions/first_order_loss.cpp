// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/types/reactions.hpp>
#include <mechanism_configuration/types/species.hpp>

#include <detail/check_schema.hpp>
#include <detail/error_format.hpp>
#include <detail/v1/reaction_parsers.hpp>
#include <detail/v1/type_parsers.hpp>
#include <detail/v1/type_schema.hpp>
#include <detail/v1/utils.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    /// @brief Checks the structural schema of a YAML-defined First order loss reaction entry.
    ///        Performs structural (schema) validation only;
    ///        and collects any errors found.
    /// @param object The YAML node representing the reaction
    /// @param existing_species Unused; semantic checks live in ValidateSemantics
    /// @param existing_phases Unused; semantic checks live in ValidateSemantics
    /// @return A list of validation errors, if any
    Errors FirstOrderLossParser::CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string_view> required_keys = { keys::reactants, keys::type, keys::gas_phase };
      std::vector<std::string_view> optional_keys = { keys::name, keys::scaling_factor, keys::products };

      Errors errors;

      auto schema_errors = mechanism_configuration::CheckSchema(object, required_keys, optional_keys);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
        return errors;
      }

      // Reactants
      schema_errors = CheckReactantsOrProductsSchema(object[keys::reactants]);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }

      if (!errors.empty())
        return errors;

      std::vector<std::pair<types::ReactionComponent, YAML::Node>> species_node_pairs;

      for (const auto& obj : object[keys::reactants])
      {
        types::ReactionComponent component;
        component.name = GetReactionComponentName(obj);
        species_node_pairs.emplace_back(component, obj);
      }

      if (species_node_pairs.size() > 1)
      {
        const auto& node = object[keys::reactants];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string message = mc_fmt::format(
            "{} error: '{}' reaction requires one reactant, but {} were provided.",
            error_location,
            object[keys::type].as<std::string>(),
            species_node_pairs.size());

        errors.push_back({ ErrorCode::TooManyReactionComponents, message });
      }

      // Semantic checks (species existence, phase membership) are performed by the
      // version-neutral ValidateSemantics over the canonical Mechanism.
      return errors;
    }

    void FirstOrderLossParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::FirstOrderLoss first_order_loss;

      first_order_loss.gas_phase = object[keys::gas_phase].as<std::string>();
      first_order_loss.reactants = ParseReactionComponent(object, keys::reactants);
      if (object[keys::products])
      {
        first_order_loss.products = ParseReactionComponents(object, keys::products);
      }
      first_order_loss.unknown_properties = GetComments(object);

      if (object[keys::scaling_factor])
      {
        first_order_loss.scaling_factor = object[keys::scaling_factor].as<double>();
      }
      if (object[keys::name])
      {
        first_order_loss.name = object[keys::name].as<std::string>();
      }

      reactions.first_order_loss.emplace_back(std::move(first_order_loss));
    }

  }  // namespace v1
}  // namespace mechanism_configuration

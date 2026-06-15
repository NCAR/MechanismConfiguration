// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <detail/v1/reaction_parsers.hpp>
#include <mechanism_configuration/types.hpp>
#include <detail/v1/type_parsers.hpp>
#include <detail/v1/type_schema.hpp>
#include <detail/v1/utils.hpp>
#include <mechanism_configuration/errors.hpp>
#include <detail/check_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    /// @brief Checks the structural schema of a YAML-defined Tunneling reaction entry
    ///        Performs structural (schema) validation only;
    ///        and collects any errors found.
    /// @param object The YAML node representing the reaction
    /// @param existing_species Unused; semantic checks live in ValidateSemantics
    /// @param existing_phases Unused; semantic checks live in ValidateSemantics
    /// @return A list of validation errors, if any
    Errors TunnelingParser::CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string_view> required_keys = {
        keys::reactants, keys::products, keys::type, keys::gas_phase
      };
      std::vector<std::string_view> optional_keys = { keys::name, keys::A, keys::B, keys::C };

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

      // Products
      schema_errors = CheckReactantsOrProductsSchema(object[keys::products]);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }

      // Semantic checks are performed by the version-neutral ValidateSemantics.

      return errors;
    }

    void TunnelingParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Tunneling tunneling;

      tunneling.gas_phase = object[keys::gas_phase].as<std::string>();
      tunneling.reactants = ParseReactionComponents(object, keys::reactants);
      tunneling.products = ParseReactionComponents(object, keys::products);
      tunneling.unknown_properties = GetComments(object);

      if (object[keys::A])
      {
        tunneling.A = object[keys::A].as<double>();
      }
      if (object[keys::B])
      {
        tunneling.B = object[keys::B].as<double>();
      }
      if (object[keys::C])
      {
        tunneling.C = object[keys::C].as<double>();
      }
      if (object[keys::name])
      {
        tunneling.name = object[keys::name].as<std::string>();
      }

      reactions.tunneling.emplace_back(std::move(tunneling));
    }

  }  // namespace v1
}  // namespace mechanism_configuration

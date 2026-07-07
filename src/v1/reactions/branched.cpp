// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/types/reactions.hpp>
#include <mechanism_configuration/types/species.hpp>

#include <detail/check_schema.hpp>
#include <detail/v1/reaction_parsers.hpp>
#include <detail/v1/type_parsers.hpp>
#include <detail/v1/type_schema.hpp>
#include <detail/v1/utils.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    /// @brief Checks the structural schema of a YAML-defined Branched reaction entry
    ///        Performs structural (schema) validation only;
    ///        and collects any errors found.
    /// @param object The YAML node representing the reaction
    /// @param existing_species Unused; semantic checks live in ValidateSemantics
    /// @param existing_phases Unused; semantic checks live in ValidateSemantics
    /// @return A list of validation errors, if any
    Errors BranchedParser::CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string_view> required_keys = {
        keys::type, keys::gas_phase, keys::reactants, keys::alkoxy_products, keys::nitrate_products
      };
      std::vector<std::string_view> optional_keys = { keys::name, keys::X, keys::Y, keys::a0, keys::n };

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

      // Alkoxy products
      schema_errors = CheckReactantsOrProductsSchema(object[keys::alkoxy_products]);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }

      // Nitrate products
      schema_errors = CheckReactantsOrProductsSchema(object[keys::nitrate_products]);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }

      // Semantic checks (species existence, phase membership) are performed by the
      // version-neutral ValidateSemantics over the canonical Mechanism.
      return errors;
    }

    void BranchedParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Branched branched;

      branched.gas_phase = object[keys::gas_phase].as<std::string>();
      branched.reactants = ParseReactionComponents(object, keys::reactants);
      branched.alkoxy_products = ParseReactionComponents(object, keys::alkoxy_products);
      branched.nitrate_products = ParseReactionComponents(object, keys::nitrate_products);
      branched.unknown_properties = GetComments(object);

      if (object[keys::X])
      {
        branched.X = object[keys::X].as<double>();
      }
      if (object[keys::Y])
      {
        branched.Y = object[keys::Y].as<double>();
      }
      if (object[keys::a0])
      {
        branched.a0 = object[keys::a0].as<double>();
      }
      if (object[keys::n])
      {
        branched.n = object[keys::n].as<double>();
      }
      if (object[keys::name])
      {
        branched.name = object[keys::name].as<std::string>();
      }

      reactions.branched.emplace_back(std::move(branched));
    }

  }  // namespace v1
}  // namespace mechanism_configuration

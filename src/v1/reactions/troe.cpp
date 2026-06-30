// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/types.hpp>

#include <detail/check_schema.hpp>
#include <detail/v1/reaction_parsers.hpp>
#include <detail/v1/type_parsers.hpp>
#include <detail/v1/type_schema.hpp>
#include <detail/v1/utils.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    /// @brief Checks the structural schema of a YAML-defined Troe reaction entry
    ///        Performs structural (schema) validation only;
    ///        and collects any errors found.
    /// @param object The YAML node representing the reaction
    /// @param existing_species Unused; semantic checks live in ValidateSemantics
    /// @param existing_phases Unused; semantic checks live in ValidateSemantics
    /// @return A list of validation errors, if any
    Errors TroeParser::CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string_view> required_keys = { keys::reactants, keys::products, keys::type, keys::gas_phase };
      std::vector<std::string_view> optional_keys = { keys::name,   keys::k0_A,   keys::k0_B, keys::k0_C, keys::kinf_A,
                                                      keys::kinf_B, keys::kinf_C, keys::Fc,   keys::N };
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

    void TroeParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Troe troe;

      troe.gas_phase = object[keys::gas_phase].as<std::string>();
      troe.reactants = ParseReactionComponents(object, keys::reactants);
      troe.products = ParseReactionComponents(object, keys::products);
      troe.unknown_properties = GetComments(object);

      if (object[keys::k0_A])
      {
        troe.k0_A = object[keys::k0_A].as<double>();
      }
      if (object[keys::k0_B])
      {
        troe.k0_B = object[keys::k0_B].as<double>();
      }
      if (object[keys::k0_C])
      {
        troe.k0_C = object[keys::k0_C].as<double>();
      }
      if (object[keys::kinf_A])
      {
        troe.kinf_A = object[keys::kinf_A].as<double>();
      }
      if (object[keys::kinf_B])
      {
        troe.kinf_B = object[keys::kinf_B].as<double>();
      }
      if (object[keys::kinf_C])
      {
        troe.kinf_C = object[keys::kinf_C].as<double>();
      }
      if (object[keys::Fc])
      {
        troe.Fc = object[keys::Fc].as<double>();
      }
      if (object[keys::N])
      {
        troe.N = object[keys::N].as<double>();
      }
      if (object[keys::name])
      {
        troe.name = object[keys::name].as<std::string>();
      }

      reactions.troe.emplace_back(std::move(troe));
    }

  }  // namespace v1
}  // namespace mechanism_configuration

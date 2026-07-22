// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/types/reactions.hpp>
#include <mechanism_configuration/types/species.hpp>

#include <detail/schema.hpp>
#include <detail/v1/keys.hpp>
#include <detail/v1/reactions/parsers.hpp>
#include <detail/v1/reactions/schema.hpp>
#include <detail/v1/utils.hpp>

namespace mechanism_configuration::v1
{
  /// @brief Checks the structural schema of a YAML-defined Ternary ChemicalActivation reaction entry
  ///        Performs structural (schema) validation only;
  ///        and collects any errors found.
  /// @param object The YAML node representing the reaction
  /// @param existing_species Unused; semantic checks live in ValidateReactionsSemantics
  /// @param existing_phases Unused; semantic checks live in ValidateReactionsSemantics
  /// @return A list of validation errors, if any
  Errors TernaryChemicalActivationParser::CheckSchema(
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

    // Semantic checks are performed by the version-neutral ValidateReactionsSemantics.

    return errors;
  }

  void TernaryChemicalActivationParser::Parse(const YAML::Node& object, types::Reactions& reactions)
  {
    types::TernaryChemicalActivation ternary;

    ternary.gas_phase = object[keys::gas_phase].as<std::string>();
    ternary.reactants = ParseReactionComponents(object, keys::reactants);
    ternary.products = ParseReactionComponents(object, keys::products);
    ternary.unknown_properties = GetComments(object);

    if (object[keys::k0_A])
    {
      ternary.k0_A = object[keys::k0_A].as<double>();
    }
    if (object[keys::k0_B])
    {
      ternary.k0_B = object[keys::k0_B].as<double>();
    }
    if (object[keys::k0_C])
    {
      ternary.k0_C = object[keys::k0_C].as<double>();
    }
    if (object[keys::kinf_A])
    {
      ternary.kinf_A = object[keys::kinf_A].as<double>();
    }
    if (object[keys::kinf_B])
    {
      ternary.kinf_B = object[keys::kinf_B].as<double>();
    }
    if (object[keys::kinf_C])
    {
      ternary.kinf_C = object[keys::kinf_C].as<double>();
    }
    if (object[keys::Fc])
    {
      ternary.Fc = object[keys::Fc].as<double>();
    }
    if (object[keys::N])
    {
      ternary.N = object[keys::N].as<double>();
    }
    if (object[keys::name])
    {
      ternary.name = object[keys::name].as<std::string>();
    }

    reactions.ternary_chemical_activation.emplace_back(std::move(ternary));
  }

}  // namespace mechanism_configuration::v1

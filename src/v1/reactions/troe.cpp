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
      std::vector<std::string_view> required_keys = {
        validation::reactants, validation::products, validation::type, validation::gas_phase
      };
      std::vector<std::string_view> optional_keys = { validation::name,   validation::k0_A,   validation::k0_B,
                                                 validation::k0_C,   validation::kinf_A, validation::kinf_B,
                                                 validation::kinf_C, validation::Fc,     validation::N };
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

      // Semantic checks are performed by the version-neutral ValidateSemantics.

      return errors;
    }

    void TroeParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Troe troe;

      troe.gas_phase = object[validation::gas_phase].as<std::string>();
      troe.reactants = ParseReactionComponents(object, validation::reactants);
      troe.products = ParseReactionComponents(object, validation::products);
      troe.unknown_properties = GetComments(object);

      if (object[validation::k0_A])
      {
        troe.k0_A = object[validation::k0_A].as<double>();
      }
      if (object[validation::k0_B])
      {
        troe.k0_B = object[validation::k0_B].as<double>();
      }
      if (object[validation::k0_C])
      {
        troe.k0_C = object[validation::k0_C].as<double>();
      }
      if (object[validation::kinf_A])
      {
        troe.kinf_A = object[validation::kinf_A].as<double>();
      }
      if (object[validation::kinf_B])
      {
        troe.kinf_B = object[validation::kinf_B].as<double>();
      }
      if (object[validation::kinf_C])
      {
        troe.kinf_C = object[validation::kinf_C].as<double>();
      }
      if (object[validation::Fc])
      {
        troe.Fc = object[validation::Fc].as<double>();
      }
      if (object[validation::N])
      {
        troe.N = object[validation::N].as<double>();
      }
      if (object[validation::name])
      {
        troe.name = object[validation::name].as<std::string>();
      }

      reactions.troe.emplace_back(std::move(troe));
    }

  }  // namespace v1
}  // namespace mechanism_configuration

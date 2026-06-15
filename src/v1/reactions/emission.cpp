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
    /// @brief Checks the structural schema of a YAML-defined Emission reaction entry.
    ///        Performs structural (schema) validation only;
    ///        and collects any errors found.
    /// @param object The YAML node representing the reaction
    /// @param existing_species Unused; semantic checks live in ValidateSemantics
    /// @param existing_phases Unused; semantic checks live in ValidateSemantics
    /// @return A list of validation errors, if any
    Errors EmissionParser::CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string_view> required_keys = { validation::products, validation::type, validation::gas_phase };
      std::vector<std::string_view> optional_keys = { validation::name, validation::scaling_factor };

      Errors errors;

      auto schema_errors = mechanism_configuration::CheckSchema(object, required_keys, optional_keys);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
        return errors;
      }

      // Products
      schema_errors = CheckReactantsOrProductsSchema(object[validation::products]);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }

      // Semantic checks (species existence, phase membership) are performed by the
      // version-neutral ValidateSemantics over the canonical Mechanism.
      return errors;
    }

    void EmissionParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Emission emission;

      emission.gas_phase = object[validation::gas_phase].as<std::string>();
      emission.products = ParseReactionComponents(object, validation::products);
      emission.unknown_properties = GetComments(object);

      if (object[validation::scaling_factor])
      {
        emission.scaling_factor = object[validation::scaling_factor].as<double>();
      }
      if (object[validation::name])
      {
        emission.name = object[validation::name].as<std::string>();
      }

      reactions.emission.emplace_back(std::move(emission));
    }

  }  // namespace v1
}  // namespace mechanism_configuration

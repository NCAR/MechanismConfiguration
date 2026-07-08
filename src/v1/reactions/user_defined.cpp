// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/types/reactions.hpp>
#include <mechanism_configuration/types/species.hpp>

#include <detail/schema.hpp>
#include <detail/v1/keys.hpp>
#include <detail/v1/reactions/parsers.hpp>
#include <detail/v1/reactions/schema.hpp>
#include <detail/v1/utils.hpp>

namespace mechanism_configuration::v1
{
  /// @brief Checks the structural schema of a YAML-defined User-defined reaction entry
  ///        Performs structural (schema) validation only;
  ///        and collects any errors found.
  /// @param object The YAML node representing the reaction
  /// @param existing_species Unused; semantic checks live in ValidateSemantics
  /// @param existing_phases Unused; semantic checks live in ValidateSemantics
  /// @return A list of validation errors, if any
  Errors UserDefinedParser::CheckSchema(
      const YAML::Node& object,
      const std::vector<types::Species>& existing_species,
      const std::vector<types::Phase>& existing_phases)
  {
    std::vector<std::string_view> required_keys = { keys::reactants, keys::products, keys::type, keys::gas_phase };
    std::vector<std::string_view> optional_keys = { keys::name, keys::scaling_factor };

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

  void UserDefinedParser::Parse(const YAML::Node& object, types::Reactions& reactions)
  {
    types::UserDefined user_defined;

    user_defined.reactants = ParseReactionComponents(object, keys::reactants);
    user_defined.products = ParseReactionComponents(object, keys::products);
    user_defined.gas_phase = object[keys::gas_phase].as<std::string>();
    user_defined.unknown_properties = GetComments(object);

    if (object[keys::scaling_factor])
    {
      user_defined.scaling_factor = object[keys::scaling_factor].as<double>();
    }

    if (object[keys::name])
    {
      user_defined.name = object[keys::name].as<std::string>();
    }

    reactions.user_defined.emplace_back(std::move(user_defined));
  }

}  // namespace mechanism_configuration::v1

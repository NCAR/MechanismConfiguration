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
  /// @brief Checks the structural schema of a YAML-defined Lambda Rate Constant reaction entry
  ///        Performs structural (schema) validation only;
  ///        and collects any errors found.
  /// @param object The YAML node representing the reaction
  /// @param existing_species Unused; semantic checks live in ValidateReactionsSemantics
  /// @param existing_phases Unused; semantic checks live in ValidateReactionsSemantics
  /// @return A list of validation errors, if any
  Errors LambdaRateConstantParser::CheckSchema(
      const YAML::Node& object,
      const std::vector<types::Species>& existing_species,
      const std::vector<types::Phase>& existing_phases)
  {
    std::vector<std::string_view> required_keys = {
      keys::reactants, keys::products, keys::type, keys::gas_phase, keys::lambda_function
    };
    std::vector<std::string_view> optional_keys = { keys::name };
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

  void LambdaRateConstantParser::Parse(const YAML::Node& object, types::Reactions& reactions)
  {
    types::LambdaRateConstant lambda_rate_constant;

    lambda_rate_constant.reactants = ParseReactionComponents(object, keys::reactants);
    lambda_rate_constant.products = ParseReactionComponents(object, keys::products);
    lambda_rate_constant.gas_phase = object[keys::gas_phase].as<std::string>();
    lambda_rate_constant.lambda_function = object[keys::lambda_function].as<std::string>();
    lambda_rate_constant.unknown_properties = GetComments(object);

    if (object[keys::name])
    {
      lambda_rate_constant.name = object[keys::name].as<std::string>();
    }

    reactions.lambda_rate_constant.emplace_back(std::move(lambda_rate_constant));
  }

}  // namespace mechanism_configuration::v1

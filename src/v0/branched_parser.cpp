// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/check_schema.hpp"
#include "detail/constants.hpp"
#include "detail/conversions.hpp"
#include "detail/v0/keys.hpp"
#include "detail/v0/parser.hpp"
#include "detail/v0/parser_types.hpp"

namespace mechanism_configuration::v0
{
  Errors BranchedParser(Mechanism& mechanism, const YAML::Node& object)
  {
    Errors errors;

    std::vector<std::string_view> required = {
      keys::TYPE, keys::REACTANTS, keys::ALKOXY_PRODUCTS, keys::NITRATE_PRODUCTS, keys::X, keys::Y, keys::A0, keys::n
    };

    auto validate = CheckSchema(object, required, {});
    errors.insert(errors.end(), validate.begin(), validate.end());
    if (validate.empty())
    {
      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> alkoxy_products;
      std::vector<types::ReactionComponent> nitrate_products;

      auto parse_error = ParseReactants(object[keys::REACTANTS], reactants);
      errors.insert(errors.end(), parse_error.begin(), parse_error.end());

      parse_error = ParseProducts(object[keys::ALKOXY_PRODUCTS], alkoxy_products);
      errors.insert(errors.end(), parse_error.begin(), parse_error.end());

      parse_error = ParseProducts(object[keys::NITRATE_PRODUCTS], nitrate_products);
      errors.insert(errors.end(), parse_error.begin(), parse_error.end());

      types::Branched parameters;
      parameters.X = object[keys::X].as<double>();
      // Account for the conversion of reactant concentrations to molecules cm-3
      int total_moles = 0;
      for (const auto& reactant : reactants)
      {
        total_moles += reactant.coefficient;
      }
      parameters.X *= std::pow(conversions::MolesM3ToMoleculesCm3, total_moles - 1);
      parameters.Y = object[keys::Y].as<double>();
      parameters.a0 = object[keys::A0].as<double>();
      parameters.n = object[keys::n].as<int>();

      parameters.reactants = reactants;
      parameters.alkoxy_products = alkoxy_products;
      parameters.nitrate_products = nitrate_products;

      mechanism.reactions.branched.push_back(parameters);
    }

    return errors;
  }
}  // namespace mechanism_configuration::v0

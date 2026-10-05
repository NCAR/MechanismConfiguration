// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/conversions.hpp"
#include "detail/schema.hpp"
#include "detail/v0/keys.hpp"
#include "detail/v0/parser.hpp"
#include "detail/v0/parser_types.hpp"

namespace mechanism_configuration::v0
{
  Errors TunnelingParser(Mechanism& mechanism, const YAML::Node& object)
  {
    Errors errors;

    std::vector<std::string_view> required = { keys::TYPE, keys::REACTANTS, keys::PRODUCTS };
    std::vector<std::string_view> optional = { keys::A, keys::B, keys::C };

    auto validate = CheckSchema(object, required, optional);
    errors.insert(errors.end(), validate.begin(), validate.end());
    if (validate.empty())
    {
      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;

      auto parse_error = ParseReactants(object[keys::REACTANTS], reactants);
      errors.insert(errors.end(), parse_error.begin(), parse_error.end());

      parse_error = ParseProducts(object[keys::PRODUCTS], products);
      errors.insert(errors.end(), parse_error.begin(), parse_error.end());

      types::Tunneling parameters;
      if (object[keys::A])
      {
        parameters.A = object[keys::A].as<double>();
      }
      // Account for the conversion of reactant concentrations to molecules cm-3
      int total_moles = 0;
      for (auto& reactant : reactants)
      {
        total_moles += reactant.coefficient;
      }
      parameters.A *= std::pow(conversions::MOLES_M3_TO_MOLECULES_CM3, total_moles - 1);
      if (object[keys::B])
      {
        parameters.B = object[keys::B].as<double>();
      }
      if (object[keys::C])
      {
        parameters.C = object[keys::C].as<double>();
      }

      parameters.reactants = reactants;
      parameters.products = products;
      mechanism.reactions.tunneling.push_back(parameters);
    }

    return errors;
  }
}  // namespace mechanism_configuration::v0

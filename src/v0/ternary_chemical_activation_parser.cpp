// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/schema.hpp"
#include "detail/constants.hpp"
#include "detail/conversions.hpp"
#include "detail/v0/keys.hpp"
#include "detail/v0/parser.hpp"
#include "detail/v0/parser_types.hpp"

namespace mechanism_configuration::v0
{
  Errors TernaryChemicalActivationParser(Mechanism& mechanism, const YAML::Node& object)
  {
    Errors errors;

    std::vector<std::string_view> required = { keys::TYPE, keys::REACTANTS, keys::PRODUCTS };
    std::vector<std::string_view> optional = { keys::K0_A,   keys::K0_B,   keys::K0_C, keys::KINF_A,
                                               keys::KINF_B, keys::KINF_C, keys::FC,   keys::N };

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

      types::TernaryChemicalActivation parameters;
      if (object[keys::K0_A])
      {
        parameters.k0_A = object[keys::K0_A].as<double>();
      }
      // Account for the conversion of reactant concentrations (including M) to molecules cm-3
      int total_moles = 0;
      for (const auto& reactant : reactants)
      {
        total_moles += reactant.coefficient;
      }
      parameters.k0_A *= std::pow(conversions::MOLES_M3_TO_MOLECULES_CM3, total_moles);
      if (object[keys::K0_B])
      {
        parameters.k0_B = object[keys::K0_B].as<double>();
      }
      if (object[keys::K0_C])
      {
        parameters.k0_C = object[keys::K0_C].as<double>();
      }
      if (object[keys::KINF_A])
      {
        parameters.kinf_A = object[keys::KINF_A].as<double>();
      }
      // Account for terms in denominator and exponent that include [M] but not other reactants
      parameters.kinf_A *= std::pow(conversions::MOLES_M3_TO_MOLECULES_CM3, total_moles - 1);
      if (object[keys::KINF_B])
      {
        parameters.kinf_B = object[keys::KINF_B].as<double>();
      }
      if (object[keys::KINF_C])
      {
        parameters.kinf_C = object[keys::KINF_C].as<double>();
      }
      if (object[keys::FC])
      {
        parameters.Fc = object[keys::FC].as<double>();
      }
      if (object[keys::N])
      {
        parameters.N = object[keys::N].as<double>();
      }

      parameters.reactants = reactants;
      parameters.products = products;
      mechanism.reactions.ternary_chemical_activation.push_back(parameters);
    }

    return errors;
  }
}  // namespace mechanism_configuration::v0

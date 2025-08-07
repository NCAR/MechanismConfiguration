// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/conversions.hpp>
#include <mechanism_configuration/v1/mechanism_parsers.hpp>
#include <mechanism_configuration/v1/reaction_parsers.hpp>
#include <mechanism_configuration/v1/reaction_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>


namespace mechanism_configuration
{
  namespace v1
  {
    Errors TernaryChemicalActivationParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions
    )
    {
      Errors errors;

      std::vector<std::string> required = { validation::type, validation::reactants, validation::products };
      std::vector<std::string> optional = { validation::k0_A,   validation::k0_B,   validation::k0_C, validation::kinf_A,
                                            validation::kinf_B, validation::kinf_C, validation::Fc,   validation::N };

      auto validate = ValidateSchema(object, required, optional);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        auto products = ParseReactantsOrProducts(validation::products, object);
        errors.insert(errors.end(), products.first.begin(), products.first.end());
        auto reactants = ParseReactantsOrProducts(validation::reactants, object);
        errors.insert(errors.end(), reactants.first.begin(), reactants.first.end());

        types::TernaryChemicalActivation parameters;
        if (object[validation::k0_A])
        {
          parameters.k0_A = object[validation::k0_A].as<double>();
        }
        // Account for the conversion of reactant concentrations (including M) to molecules cm-3
        int total_moles = 0;
        for (const auto& reactant : reactants.second)
        {
          total_moles += reactant.coefficient;
        }
        parameters.k0_A *= std::pow(conversions::MolesM3ToMoleculesCm3, total_moles);
        if (object[validation::k0_B])
        {
          parameters.k0_B = object[validation::k0_B].as<double>();
        }
        if (object[validation::k0_C])
        {
          parameters.k0_C = object[validation::k0_C].as<double>();
        }
        if (object[validation::kinf_A])
        {
          parameters.kinf_A = object[validation::kinf_A].as<double>();
        }
        // Account for terms in denominator and exponent that include [M] but not other reactants
        parameters.kinf_A *= std::pow(conversions::MolesM3ToMoleculesCm3, total_moles - 1);
        if (object[validation::kinf_B])
        {
          parameters.kinf_B = object[validation::kinf_B].as<double>();
        }
        if (object[validation::kinf_C])
        {
          parameters.kinf_C = object[validation::kinf_C].as<double>();
        }
        if (object[validation::Fc])
        {
          parameters.Fc = object[validation::Fc].as<double>();
        }
        if (object[validation::N])
        {
          parameters.N = object[validation::N].as<double>();
        }

        parameters.reactants = reactants.second;
        parameters.products = products.second;
        reactions.ternary_chemical_activation.push_back(parameters);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

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
        types::Reactions& reactions)
    {
      Errors errors;

      std::vector<std::string> required_keys = {
        validation::products, validation::reactants, validation::type, validation::gas_phase
      };
      std::vector<std::string> optional_keys = { validation::name,   validation::k0_A,   validation::k0_B,
                                                 validation::k0_C,   validation::kinf_A, validation::kinf_B,
                                                 validation::kinf_C, validation::Fc,     validation::N };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
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

        if (object[validation::name])
        {
          parameters.name = object[validation::name].as<std::string>();
        }

        std::string gas_phase = object[validation::gas_phase].as<std::string>();
        auto it = std::find_if(
            existing_phases.begin(),
            existing_phases.end(),
            [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::gas_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::gas_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + gas_phase });
        }

        parameters.gas_phase = gas_phase;
        parameters.reactants = reactants.second;
        parameters.products = products.second;
        parameters.unknown_properties = GetComments(object);
        reactions.ternary_chemical_activation.push_back(parameters);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

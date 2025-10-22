// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/mechanism_parsers.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/utils.hpp>

namespace mechanism_configuration
{
  namespace development
  {
    void TernaryChemicalActivationParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::TernaryChemicalActivation ternary;

      for (const auto& elem : object[validation::reactants])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        ternary.reactants.emplace_back(std::move(component));
      }

      for (const auto& elem : object[validation::products])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        ternary.products.emplace_back(std::move(component));
      }

      if (object[validation::k0_A].IsDefined())
      {
        ternary.k0_A = object[validation::k0_A].as<double>();
      }
      if (object[validation::k0_B].IsDefined())
      {
        ternary.k0_B = object[validation::k0_B].as<double>();
      }
      if (object[validation::k0_C].IsDefined())
      {
        ternary.k0_C = object[validation::k0_C].as<double>();
      }
      if (object[validation::kinf_A].IsDefined())
      {
        ternary.kinf_A = object[validation::kinf_A].as<double>();
      }
      if (object[validation::kinf_B].IsDefined())
      {
        ternary.kinf_B = object[validation::kinf_B].as<double>();
      }
      if (object[validation::kinf_C].IsDefined())
      {
        ternary.kinf_C = object[validation::kinf_C].as<double>();
      }
      if (object[validation::Fc].IsDefined())
      {
        ternary.Fc = object[validation::Fc].as<double>();
      }
      if (object[validation::N].IsDefined())
      {
        ternary.N = object[validation::N].as<double>();
      }

      if (object[validation::name].IsDefined())
      {
        ternary.name = object[validation::name].as<std::string>();
      }

      ternary.gas_phase = object[validation::gas_phase].as<std::string>();
      ternary.unknown_properties = GetComments(object);
      reactions.ternary_chemical_activation.emplace_back(std::move(ternary));
    }

  }  // namespace development
}  // namespace mechanism_configuration

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
    void AqueousEquilibriumParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::AqueousEquilibrium aqueous_equilibrium;

      for (const auto& elem : object[validation::reactants])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        aqueous_equilibrium.reactants.emplace_back(std::move(component));
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
        aqueous_equilibrium.products.emplace_back(std::move(component));
      }

      if (object[validation::A].IsDefined())
      {
        aqueous_equilibrium.A = object[validation::A].as<double>();
      }
      if (object[validation::C].IsDefined())
      {
        aqueous_equilibrium.C = object[validation::C].as<double>();
      }

      if (object[validation::name].IsDefined())
      {
        aqueous_equilibrium.name = object[validation::name].as<std::string>();
      }

      aqueous_equilibrium.condensed_phase = object[validation::condensed_phase].as<std::string>();
      aqueous_equilibrium.condensed_phase_water = object[validation::condensed_phase_water].as<std::string>();
      aqueous_equilibrium.k_reverse = object[validation::k_reverse].as<double>();
      aqueous_equilibrium.unknown_properties = GetComments(object);

      reactions.aqueous_equilibrium.emplace_back(std::move(aqueous_equilibrium));
    }

  }  // namespace development
}  // namespace mechanism_configuration

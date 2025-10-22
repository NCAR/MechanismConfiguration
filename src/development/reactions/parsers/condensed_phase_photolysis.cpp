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
    void CondensedPhasePhotolysisParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::CondensedPhasePhotolysis condensed_phase_photolysis;

      for (const auto& elem : object[validation::reactants])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        condensed_phase_photolysis.reactants.emplace_back(std::move(component));
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
        condensed_phase_photolysis.products.emplace_back(std::move(component));
      }

      if (object[validation::scaling_factor].IsDefined())
      {
        condensed_phase_photolysis.scaling_factor = object[validation::scaling_factor].as<double>();
      }
      if (object[validation::name].IsDefined())
      {
        condensed_phase_photolysis.name = object[validation::name].as<std::string>();
      }

      condensed_phase_photolysis.condensed_phase = object[validation::condensed_phase].as<std::string>();
      condensed_phase_photolysis.unknown_properties = GetComments(object);
      reactions.condensed_phase_photolysis.emplace_back(std::move(condensed_phase_photolysis));
    }

  }  // namespace development
}  // namespace mechanism_configuration

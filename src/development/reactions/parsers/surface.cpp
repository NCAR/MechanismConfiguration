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
    void SurfaceParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Surface surface;

      for (const auto& elem : object[validation::gas_phase_species])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        surface.gas_phase_species.emplace_back(std::move(component));
      }

      for (const auto& elem : object[validation::gas_phase_products])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        surface.gas_phase_products.emplace_back(std::move(component));
      }

      if (object[validation::reaction_probability].IsDefined())
      {
        surface.reaction_probability = object[validation::reaction_probability].as<double>();
      }
      if (object[validation::name].IsDefined())
      {
        surface.name = object[validation::name].as<std::string>();
      }

      surface.gas_phase = object[validation::gas_phase].as<std::string>();
      surface.condensed_phase = object[validation::condensed_phase].as<std::string>();
      surface.unknown_properties = GetComments(object);
      reactions.surface.emplace_back(std::move(surface));
    }

  }  // namespace development
}  // namespace mechanism_configuration

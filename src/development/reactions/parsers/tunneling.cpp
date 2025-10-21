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
    void TunnelingParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Tunneling tunneling;

      for (const auto& elem : object[validation::reactants])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        tunneling.reactants.emplace_back(std::move(component));
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
        tunneling.products.emplace_back(std::move(component));
      }

      if (object[validation::A].IsDefined())
      {
        tunneling.A = object[validation::A].as<double>();
      }
      if (object[validation::B].IsDefined())
      {
        tunneling.B = object[validation::B].as<double>();
      }
      if (object[validation::C].IsDefined())
      {
        tunneling.C = object[validation::C].as<double>();
      }

      if (object[validation::name].IsDefined())
      {
        tunneling.name = object[validation::name].as<std::string>();
      }

      tunneling.gas_phase = object[validation::gas_phase].as<std::string>();
      tunneling.unknown_properties = GetComments(object);

      reactions.tunneling.emplace_back(std::move(tunneling));
    }

  }  // namespace development
}  // namespace mechanism_configuration

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
    void EmissionParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Emission emission;

      for (const auto& elem : object[validation::products])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient])
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        emission.products.emplace_back(std::move(component));
      }

      if (object[validation::scaling_factor])
      {
        emission.scaling_factor = object[validation::scaling_factor].as<double>();
      }
      if (object[validation::name])
      {
        emission.name = object[validation::name].as<std::string>();
      }

      emission.gas_phase = object[validation::gas_phase].as<std::string>();
      emission.unknown_properties = GetComments(object);

      reactions.emission.emplace_back(std::move(emission));
    }

  }  // namespace development
}  // namespace mechanism_configuration

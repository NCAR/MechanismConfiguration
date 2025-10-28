// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/development/mechanism_parsers.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/utils.hpp>

namespace mechanism_configuration
{
  namespace development
  {
    void UserDefinedParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::UserDefined user_defined;

      for (const auto& elem : object[validation::reactants])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient])
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        user_defined.reactants.emplace_back(std::move(component));
      }

      for (const auto& elem : object[validation::products])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient])
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        user_defined.products.emplace_back(std::move(component));
      }

      if (object[validation::scaling_factor])
      {
        user_defined.scaling_factor = object[validation::scaling_factor].as<double>();
      }

      if (object[validation::name])
      {
        user_defined.name = object[validation::name].as<std::string>();
      }

      user_defined.gas_phase = object[validation::gas_phase].as<std::string>();
      user_defined.unknown_properties = GetComments(object);
      reactions.user_defined.emplace_back(std::move(user_defined));
    }

  }  // namespace development
}  // namespace mechanism_configuration

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
    void FirstOrderLossParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::FirstOrderLoss first_order_loss;

      for (const auto& elem : object[validation::reactants])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient])
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        first_order_loss.reactants.emplace_back(std::move(component));
      }

      if (object[validation::scaling_factor])
      {
        first_order_loss.scaling_factor = object[validation::scaling_factor].as<double>();
      }
      if (object[validation::name])
      {
        first_order_loss.name = object[validation::name].as<std::string>();
      }

      first_order_loss.gas_phase = object[validation::gas_phase].as<std::string>();
      first_order_loss.unknown_properties = GetComments(object);

      reactions.first_order_loss.emplace_back(std::move(first_order_loss));
    }

  }  // namespace development
}  // namespace mechanism_configuration

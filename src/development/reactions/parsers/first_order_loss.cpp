// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/type_parsers.hpp>
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

      first_order_loss.gas_phase = object[validation::gas_phase].as<std::string>();
      first_order_loss.reactants = ParseReactionComponent(object, validation::reactants);
      first_order_loss.unknown_properties = GetComments(object);

      if (object[validation::scaling_factor])
      {
        first_order_loss.scaling_factor = object[validation::scaling_factor].as<double>();
      }
      if (object[validation::name])
      {
        first_order_loss.name = object[validation::name].as<std::string>();
      }

      reactions.first_order_loss.emplace_back(std::move(first_order_loss));
    }

  }  // namespace development
}  // namespace mechanism_configuration

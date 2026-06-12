// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/v1/reaction_parsers.hpp>
#include <mechanism_configuration/types.hpp>
#include <mechanism_configuration/v1/type_parsers.hpp>
#include <mechanism_configuration/v1/utils.hpp>

namespace mechanism_configuration
{
  namespace v1
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

  }  // namespace v1
}  // namespace mechanism_configuration

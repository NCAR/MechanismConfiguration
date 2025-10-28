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

      tunneling.gas_phase = object[validation::gas_phase].as<std::string>();
      tunneling.reactants = ParseReactionComponents(object, validation::reactants);
      tunneling.products = ParseReactionComponents(object, validation::products);
      tunneling.unknown_properties = GetComments(object);

      if (object[validation::A])
      {
        tunneling.A = object[validation::A].as<double>();
      }
      if (object[validation::B])
      {
        tunneling.B = object[validation::B].as<double>();
      }
      if (object[validation::C])
      {
        tunneling.C = object[validation::C].as<double>();
      }
      if (object[validation::name])
      {
        tunneling.name = object[validation::name].as<std::string>();
      }

      reactions.tunneling.emplace_back(std::move(tunneling));
    }

  }  // namespace development
}  // namespace mechanism_configuration

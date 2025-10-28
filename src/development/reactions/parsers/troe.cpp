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
    void TroeParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Troe troe;

      troe.gas_phase = object[validation::gas_phase].as<std::string>();
      troe.reactants = ParseReactionComponents(object, validation::reactants);
      troe.products = ParseReactionComponents(object, validation::products);
      troe.unknown_properties = GetComments(object);

      if (object[validation::k0_A])
      {
        troe.k0_A = object[validation::k0_A].as<double>();
      }
      if (object[validation::k0_B])
      {
        troe.k0_B = object[validation::k0_B].as<double>();
      }
      if (object[validation::k0_C])
      {
        troe.k0_C = object[validation::k0_C].as<double>();
      }
      if (object[validation::kinf_A])
      {
        troe.kinf_A = object[validation::kinf_A].as<double>();
      }
      if (object[validation::kinf_B])
      {
        troe.kinf_B = object[validation::kinf_B].as<double>();
      }
      if (object[validation::kinf_C])
      {
        troe.kinf_C = object[validation::kinf_C].as<double>();
      }
      if (object[validation::Fc])
      {
        troe.Fc = object[validation::Fc].as<double>();
      }
      if (object[validation::N])
      {
        troe.N = object[validation::N].as<double>();
      }
      if (object[validation::name])
      {
        troe.name = object[validation::name].as<std::string>();
      }

      reactions.troe.emplace_back(std::move(troe));
    }

  }  // namespace development
}  // namespace mechanism_configuration

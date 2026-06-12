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
    void AqueousEquilibriumParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::AqueousEquilibrium aqueous_equilibrium;

      aqueous_equilibrium.condensed_phase = object[validation::condensed_phase].as<std::string>();
      aqueous_equilibrium.condensed_phase_water = object[validation::condensed_phase_water].as<std::string>();
      aqueous_equilibrium.k_reverse = object[validation::k_reverse].as<double>();
      aqueous_equilibrium.reactants = ParseReactionComponents(object, validation::reactants);
      aqueous_equilibrium.products = ParseReactionComponents(object, validation::products);
      aqueous_equilibrium.unknown_properties = GetComments(object);

      if (object[validation::A])
      {
        aqueous_equilibrium.A = object[validation::A].as<double>();
      }
      if (object[validation::C])
      {
        aqueous_equilibrium.C = object[validation::C].as<double>();
      }
      if (object[validation::name])
      {
        aqueous_equilibrium.name = object[validation::name].as<std::string>();
      }

      reactions.aqueous_equilibrium.emplace_back(std::move(aqueous_equilibrium));
    }

  }  // namespace v1
}  // namespace mechanism_configuration

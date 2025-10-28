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
    void CondensedPhaseArrheniusParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::CondensedPhaseArrhenius condensed_phase_arrhenius;

      condensed_phase_arrhenius.condensed_phase = object[validation::condensed_phase].as<std::string>();
      condensed_phase_arrhenius.reactants = ParseReactionComponents(object, validation::reactants);
      condensed_phase_arrhenius.products = ParseReactionComponents(object, validation::products);
      condensed_phase_arrhenius.unknown_properties = GetComments(object);

      if (object[validation::A])
      {
        condensed_phase_arrhenius.A = object[validation::A].as<double>();
      }
      if (object[validation::B])
      {
        condensed_phase_arrhenius.B = object[validation::B].as<double>();
      }
      if (object[validation::C])
      {
        condensed_phase_arrhenius.C = object[validation::C].as<double>();
      }
      if (object[validation::D])
      {
        condensed_phase_arrhenius.D = object[validation::D].as<double>();
      }
      if (object[validation::E])
      {
        condensed_phase_arrhenius.E = object[validation::E].as<double>();
      }
      if (object[validation::Ea])
      {
        condensed_phase_arrhenius.C = -1 * object[validation::Ea].as<double>() / constants::boltzmann;
      }
      if (object[validation::name])
      {
        condensed_phase_arrhenius.name = object[validation::name].as<std::string>();
      }

      reactions.condensed_phase_arrhenius.emplace_back(std::move(condensed_phase_arrhenius));
    }

  }  // namespace development
}  // namespace mechanism_configuration

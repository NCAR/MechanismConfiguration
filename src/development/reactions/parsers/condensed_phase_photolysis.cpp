// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/reaction_parsers.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/type_parsers.hpp>
#include <mechanism_configuration/development/utils.hpp>

namespace mechanism_configuration
{
  namespace development
  {
    void CondensedPhasePhotolysisParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::CondensedPhasePhotolysis condensed_phase_photolysis;

      condensed_phase_photolysis.condensed_phase = object[validation::condensed_phase].as<std::string>();
      condensed_phase_photolysis.reactants = ParseReactionComponent(object, validation::reactants);
      condensed_phase_photolysis.products = ParseReactionComponents(object, validation::products);
      condensed_phase_photolysis.unknown_properties = GetComments(object);

      if (object[validation::scaling_factor])
      {
        condensed_phase_photolysis.scaling_factor = object[validation::scaling_factor].as<double>();
      }
      if (object[validation::name])
      {
        condensed_phase_photolysis.name = object[validation::name].as<std::string>();
      }

      reactions.condensed_phase_photolysis.emplace_back(std::move(condensed_phase_photolysis));
    }

  }  // namespace development
}  // namespace mechanism_configuration

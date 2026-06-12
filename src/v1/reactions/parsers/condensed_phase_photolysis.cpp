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

  }  // namespace v1
}  // namespace mechanism_configuration

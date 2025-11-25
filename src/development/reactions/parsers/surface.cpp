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
    void SurfaceParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Surface surface;

      surface.gas_phase = object[validation::gas_phase].as<std::string>();
      surface.condensed_phase = object[validation::condensed_phase].as<std::string>();
      surface.gas_phase_species = ParseReactionComponent(object, validation::gas_phase_species);
      surface.gas_phase_products = ParseReactionComponents(object, validation::gas_phase_products);
      surface.unknown_properties = GetComments(object);

      if (object[validation::reaction_probability])
      {
        surface.reaction_probability = object[validation::reaction_probability].as<double>();
      }
      if (object[validation::name])
      {
        surface.name = object[validation::name].as<std::string>();
      }

      reactions.surface.emplace_back(std::move(surface));
    }

  }  // namespace development
}  // namespace mechanism_configuration

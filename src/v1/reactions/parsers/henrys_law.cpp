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
    void HenrysLawParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::HenrysLaw henrys_law;

      // Parse gas
      types::Phase gas;
      gas.name = object[validation::gas][validation::name].as<std::string>();

      for (const auto& elem : object[validation::gas][validation::species])
      {
        types::PhaseSpecies phase_species;
        phase_species.name = elem[validation::name].as<std::string>();
        phase_species.unknown_properties = GetComments(elem);
        if (elem[validation::diffusion_coefficient])
        {
          phase_species.diffusion_coefficient = elem[validation::diffusion_coefficient].as<double>();
        }
        gas.species.emplace_back(std::move(phase_species));
      }

      henrys_law.gas = std::move(gas);
      henrys_law.particle.phase = object[validation::particle][validation::phase].as<std::string>();
      henrys_law.particle.solutes = ParseReactionComponents(object[validation::particle], validation::solutes);
      henrys_law.particle.solvent = ParseReactionComponent(object[validation::particle], validation::solvent);
      henrys_law.unknown_properties = GetComments(object);

      if (object[validation::name])
      {
        henrys_law.name = object[validation::name].as<std::string>();
      }

      reactions.henrys_law.emplace_back(std::move(henrys_law));
    }

  }  // namespace v1
}  // namespace mechanism_configuration
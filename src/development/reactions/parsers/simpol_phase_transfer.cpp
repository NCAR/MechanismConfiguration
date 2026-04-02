// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
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
    void SimpolPhaseTransferParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::SimpolPhaseTransfer simpol_phase_transfer;

      simpol_phase_transfer.gas_phase = object[validation::gas_phase].as<std::string>();
      simpol_phase_transfer.condensed_phase = object[validation::condensed_phase].as<std::string>();
      simpol_phase_transfer.gas_phase_species = ParseReactionComponents(object, validation::gas_phase_species);
      simpol_phase_transfer.condensed_phase_species = ParseReactionComponents(object, validation::condensed_phase_species);
      simpol_phase_transfer.unknown_properties = GetComments(object);

      for (size_t i = 0; i < 4; ++i)
      {
        simpol_phase_transfer.B[i] = object[validation::B][i].as<double>();
      }

      if (object[validation::name])
      {
        simpol_phase_transfer.name = object[validation::name].as<std::string>();
      }

      reactions.simpol_phase_transfer.emplace_back(std::move(simpol_phase_transfer));
    }

  }  // namespace development
}  // namespace mechanism_configuration

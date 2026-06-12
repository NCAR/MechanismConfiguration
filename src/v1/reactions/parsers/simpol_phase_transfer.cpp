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

  }  // namespace v1
}  // namespace mechanism_configuration

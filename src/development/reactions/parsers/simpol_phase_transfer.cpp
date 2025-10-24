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
    void SimpolPhaseTransferParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::SimpolPhaseTransfer simpol_phase_transfer;

      for (const auto& elem : object[validation::gas_phase_species])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        simpol_phase_transfer.gas_phase_species.emplace_back(std::move(component));
      }

      for (const auto& elem : object[validation::condensed_phase_species])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        simpol_phase_transfer.condensed_phase_species.emplace_back(std::move(component));
      }

      for (size_t i = 0; i < 4; ++i)
      {
        simpol_phase_transfer.B[i] = object[validation::B][i].as<double>();
      }

      if (object[validation::name].IsDefined())
      {
        simpol_phase_transfer.name = object[validation::name].as<std::string>();
      }

      simpol_phase_transfer.gas_phase = object[validation::gas_phase].as<std::string>();
      simpol_phase_transfer.condensed_phase = object[validation::condensed_phase].as<std::string>();
      simpol_phase_transfer.unknown_properties = GetComments(object);

      reactions.simpol_phase_transfer.emplace_back(std::move(simpol_phase_transfer));
    }

  }  // namespace development
}  // namespace mechanism_configuration

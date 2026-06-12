// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/v1/reaction_parsers.hpp>
#include <mechanism_configuration/types.hpp>
#include <mechanism_configuration/types.hpp>
#include <mechanism_configuration/v1/utils.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    void WetDepositionParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::WetDeposition wet_deposition;

      wet_deposition.condensed_phase = object[validation::condensed_phase].as<std::string>();
      wet_deposition.unknown_properties = GetComments(object);

      if (object[validation::scaling_factor])
      {
        wet_deposition.scaling_factor = object[validation::scaling_factor].as<double>();
      }

      if (object[validation::name])
      {
        wet_deposition.name = object[validation::name].as<std::string>();
      }

      reactions.wet_deposition.emplace_back(std::move(wet_deposition));
    }

  }  // namespace v1
}  // namespace mechanism_configuration
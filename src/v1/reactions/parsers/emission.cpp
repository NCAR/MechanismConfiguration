// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <detail/v1/reaction_parsers.hpp>
#include <mechanism_configuration/types.hpp>
#include <detail/v1/type_parsers.hpp>
#include <detail/v1/utils.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    void EmissionParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Emission emission;

      emission.gas_phase = object[validation::gas_phase].as<std::string>();
      emission.products = ParseReactionComponents(object, validation::products);
      emission.unknown_properties = GetComments(object);

      if (object[validation::scaling_factor])
      {
        emission.scaling_factor = object[validation::scaling_factor].as<double>();
      }
      if (object[validation::name])
      {
        emission.name = object[validation::name].as<std::string>();
      }

      reactions.emission.emplace_back(std::move(emission));
    }

  }  // namespace v1
}  // namespace mechanism_configuration

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
    void PhotolysisParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Photolysis photolysis;

      photolysis.gas_phase = object[validation::gas_phase].as<std::string>();
      photolysis.reactants = ParseReactionComponent(object, validation::reactants);
      photolysis.products = ParseReactionComponents(object, validation::products);
      photolysis.unknown_properties = GetComments(object);

      if (object[validation::scaling_factor])
      {
        photolysis.scaling_factor = object[validation::scaling_factor].as<double>();
      }
      if (object[validation::name])
      {
        photolysis.name = object[validation::name].as<std::string>();
      }

      reactions.photolysis.emplace_back(std::move(photolysis));
    }

  }  // namespace v1
}  // namespace mechanism_configuration

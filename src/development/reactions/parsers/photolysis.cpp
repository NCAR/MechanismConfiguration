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

  }  // namespace development
}  // namespace mechanism_configuration

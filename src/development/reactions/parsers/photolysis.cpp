// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/mechanism_parsers.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace development
  {
    void PhotolysisParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Photolysis photolysis;

      for (const auto& elem : object[validation::reactants])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        photolysis.reactants.emplace_back(std::move(component));
      }

      for (const auto& elem : object[validation::products])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        photolysis.products.emplace_back(std::move(component));
      }

      if (object[validation::scaling_factor].IsDefined())
      {
        photolysis.scaling_factor = object[validation::scaling_factor].as<double>();
      }
      if (object[validation::name].IsDefined())
      {
        photolysis.name = object[validation::name].as<std::string>();
      }

      photolysis.gas_phase = object[validation::gas_phase].as<std::string>();
      photolysis.unknown_properties = GetComments(object);

      reactions.photolysis.emplace_back(std::move(photolysis));
    }

  }  // namespace development
}  // namespace mechanism_configuration

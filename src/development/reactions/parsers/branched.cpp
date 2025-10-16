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
    void BranchedParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Branched branched;

      for (const auto& elem : object[validation::reactants])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        branched.reactants.emplace_back(std::move(component));
      }

      for (const auto& elem : object[validation::alkoxy_products])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        branched.alkoxy_products.emplace_back(std::move(component));
      }

      for (const auto& elem : object[validation::nitrate_products])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient].IsDefined())
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        branched.nitrate_products.emplace_back(std::move(component));
      }

      if (object[validation::X].IsDefined())
      {
        branched.X = object[validation::X].as<double>();
      }
      if (object[validation::Y].IsDefined())
      {
        branched.Y = object[validation::Y].as<double>();
      }
      if (object[validation::a0].IsDefined())
      {
        branched.a0 = object[validation::a0].as<double>();
      }
      if (object[validation::n].IsDefined())
      {
        branched.n = object[validation::n].as<double>();
      }
      if (object[validation::name].IsDefined())
      {
        branched.name = object[validation::name].as<std::string>();
      }

      branched.gas_phase = object[validation::gas_phase].as<std::string>();
      branched.unknown_properties = GetComments(object);

      reactions.branched.emplace_back(std::move(branched));
    }

  }  // namespace development
}  // namespace mechanism_configuration

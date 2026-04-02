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
    void BranchedParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Branched branched;

      branched.gas_phase = object[validation::gas_phase].as<std::string>();
      branched.reactants = ParseReactionComponents(object, validation::reactants);
      branched.alkoxy_products = ParseReactionComponents(object, validation::alkoxy_products);
      branched.nitrate_products = ParseReactionComponents(object, validation::nitrate_products);
      branched.unknown_properties = GetComments(object);

      if (object[validation::X])
      {
        branched.X = object[validation::X].as<double>();
      }
      if (object[validation::Y])
      {
        branched.Y = object[validation::Y].as<double>();
      }
      if (object[validation::a0])
      {
        branched.a0 = object[validation::a0].as<double>();
      }
      if (object[validation::n])
      {
        branched.n = object[validation::n].as<double>();
      }
      if (object[validation::name])
      {
        branched.name = object[validation::name].as<std::string>();
      }

      reactions.branched.emplace_back(std::move(branched));
    }

  }  // namespace development
}  // namespace mechanism_configuration

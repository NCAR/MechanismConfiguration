// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/type_parsers.hpp>
#include <mechanism_configuration/development/utils.hpp>

namespace mechanism_configuration
{
  namespace development
  {
    void UserDefinedParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::UserDefined user_defined;

      user_defined.reactants = ParseReactionComponents(object, validation::reactants);
      user_defined.products = ParseReactionComponents(object, validation::products);
      user_defined.gas_phase = object[validation::gas_phase].as<std::string>();
      user_defined.unknown_properties = GetComments(object);

      if (object[validation::scaling_factor])
      {
        user_defined.scaling_factor = object[validation::scaling_factor].as<double>();
      }

      if (object[validation::name])
      {
        user_defined.name = object[validation::name].as<std::string>();
      }

      reactions.user_defined.emplace_back(std::move(user_defined));
    }

  }  // namespace development
}  // namespace mechanism_configuration

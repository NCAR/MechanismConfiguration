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
    void LambdaRateConstantParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::LambdaRateConstant lambda_rate_constant;

      lambda_rate_constant.reactants = ParseReactionComponents(object, validation::reactants);
      lambda_rate_constant.products = ParseReactionComponents(object, validation::products);
      lambda_rate_constant.gas_phase = object[validation::gas_phase].as<std::string>();
      lambda_rate_constant.lambda_function = object[validation::lambda_function].as<std::string>();
      lambda_rate_constant.unknown_properties = GetComments(object);

      if (object[validation::name])
      {
        lambda_rate_constant.name = object[validation::name].as<std::string>();
      }

      reactions.lambda_rate_constant.emplace_back(std::move(lambda_rate_constant));
    }

  }  // namespace development
}  // namespace mechanism_configuration

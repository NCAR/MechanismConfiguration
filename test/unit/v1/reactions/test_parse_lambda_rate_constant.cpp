// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/v1/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidLambdaRateConstantReaction)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/lambda_rate_constant/valid") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.lambda_rate_constant.size(), 2);

    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[0].name, "my lambda reaction");
    EXPECT_EQ(
        mechanism.reactions.lambda_rate_constant[0].lambda_function,
        "[](double T, double P) { return 1.2e-5 * exp(-500.0 / T); }");
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[0].reactants[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[0].products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[0].unknown_properties["__comment"], "hi");

    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[1].name, "");
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[1].lambda_function, "[](double T) { return 3.0e-4 * T; }");
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[1].reactants.size(), 2);
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[1].reactants[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[1].reactants[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[1].reactants[1].species_name, "A");
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[1].reactants[1].coefficient, 0.5);
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[1].products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.lambda_rate_constant[1].products[0].coefficient, 0.2);
  }
}

TEST(ParserBase, LambdaRateConstantDetectsUnknownSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/lambda_rate_constant/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, LambdaRateConstantDetectsBadReactionComponent)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/lambda_rate_constant/bad_reaction_component") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::InvalidKey);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, LambdaRateConstantDetectsUnknownPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/lambda_rate_constant/missing_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::UnknownPhase);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

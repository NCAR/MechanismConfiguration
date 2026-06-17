// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidUserDefinedReaction)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = Parse(std::string("v1_unit_configs/reactions/user_defined/valid") + extension);
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.user_defined.size(), 2);

    EXPECT_EQ(mechanism.reactions.user_defined[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.user_defined[0].name, "my user defined");
    EXPECT_EQ(mechanism.reactions.user_defined[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.user_defined[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].reactants[0].name, "B");
    EXPECT_EQ(mechanism.reactions.user_defined[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.user_defined[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].unknown_properties["__comment"], "hi");

    EXPECT_EQ(mechanism.reactions.user_defined[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.user_defined[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants.size(), 2);
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants[0].name, "B");
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants[1].name, "A");
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants[1].coefficient, 0.5);
    EXPECT_EQ(mechanism.reactions.user_defined[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined[1].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.user_defined[1].products[0].coefficient, 0.2);
  }
}

TEST(ParserBase, UserDefinedDetectsUnknownSpecies)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/user_defined/unknown_species") + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::ReactionRequiresUnknownSpecies);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, UserDefinedDetectsBadReactionComponent)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/user_defined/bad_reaction_component") + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::InvalidKey);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, UserDefinedDetectsUnknownPhase)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/user_defined/missing_phase") + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::UnknownPhase);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

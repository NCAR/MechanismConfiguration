// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "utils/print.hpp"

#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidBranchedReaction)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = Parse(std::string("v1_unit_configs/reactions/branched/valid") + extension);
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.branched.size(), 1);

    EXPECT_EQ(mechanism.reactions.branched[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.branched[0].name, "my branched");
    EXPECT_EQ(mechanism.reactions.branched[0].X, 1.2e-4);
    EXPECT_EQ(mechanism.reactions.branched[0].Y, 167);
    EXPECT_EQ(mechanism.reactions.branched[0].a0, 0.15);
    EXPECT_EQ(mechanism.reactions.branched[0].n, 9);
    EXPECT_EQ(mechanism.reactions.branched[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched[0].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.branched[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products[0].unknown_properties["__thing"], "hi");
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products.size(), 2);
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products[0].name, "B");
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products[0].coefficient, 0.2);
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products[1].name, "A");
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products[1].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.branched[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched[0].unknown_properties["__comment"], "thing");
  }
}

TEST(ParserBase, BranchedDetectsUnknownSpecies)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/branched/unknown_species") + extension;
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

TEST(ParserBase, BranchedDetectsBadReactionComponent)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/branched/bad_reaction_component") + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 2);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::RequiredKeyNotFound);
    EXPECT_EQ(parsed.error()[1].first, ErrorCode::InvalidKey);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, BranchedDetectsUnknownPhase)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/branched/missing_phase") + extension;
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

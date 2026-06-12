#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidTroeReaction)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parse(std::string("v1_unit_configs/reactions/troe/valid") + extension);
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.troe.size(), 2);

    EXPECT_EQ(mechanism.reactions.troe[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.troe[0].k0_A, 1.0);
    EXPECT_EQ(mechanism.reactions.troe[0].k0_B, 0.0);
    EXPECT_EQ(mechanism.reactions.troe[0].k0_C, 0.0);
    EXPECT_EQ(mechanism.reactions.troe[0].kinf_A, 1.0);
    EXPECT_EQ(mechanism.reactions.troe[0].kinf_B, 0.0);
    EXPECT_EQ(mechanism.reactions.troe[0].kinf_C, 0.0);
    EXPECT_EQ(mechanism.reactions.troe[0].Fc, 0.6);
    EXPECT_EQ(mechanism.reactions.troe[0].N, 1.0);
    EXPECT_EQ(mechanism.reactions.troe[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe[0].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.troe[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.troe[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe[0].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.troe[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.troe[0].unknown_properties.size(), 1);
    if (extension == ".json")
    {
      EXPECT_EQ(mechanism.reactions.troe[0].unknown_properties["__my object"], "{a: 1.0}");
    }
    else
    {
      EXPECT_EQ(mechanism.reactions.troe[0].unknown_properties["__my object"], "a: 1.0");
    }

    EXPECT_EQ(mechanism.reactions.troe[1].name, "my troe");
    EXPECT_EQ(mechanism.reactions.troe[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.troe[1].k0_A, 32.1);
    EXPECT_EQ(mechanism.reactions.troe[1].k0_B, -2.3);
    EXPECT_EQ(mechanism.reactions.troe[1].k0_C, 102.3);
    EXPECT_EQ(mechanism.reactions.troe[1].kinf_A, 63.4);
    EXPECT_EQ(mechanism.reactions.troe[1].kinf_B, -1.3);
    EXPECT_EQ(mechanism.reactions.troe[1].kinf_C, 908.5);
    EXPECT_EQ(mechanism.reactions.troe[1].Fc, 1.3);
    EXPECT_EQ(mechanism.reactions.troe[1].N, 32.1);
    EXPECT_EQ(mechanism.reactions.troe[1].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe[1].reactants[0].name, "C");
    EXPECT_EQ(mechanism.reactions.troe[1].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.troe[1].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.troe[1].products[0].name, "A");
    EXPECT_EQ(mechanism.reactions.troe[1].products[0].coefficient, 0.2);
    EXPECT_EQ(mechanism.reactions.troe[1].products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe[1].products[0].unknown_properties["__optional thing"], "hello");
    EXPECT_EQ(mechanism.reactions.troe[1].products[1].name, "B");
    EXPECT_EQ(mechanism.reactions.troe[1].products[1].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.troe[1].products[1].unknown_properties.size(), 0);
  }
}

TEST(ParserBase, TroeDetectsUnknownSpecies)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/troe/unknown_species") + extension;
    auto parsed = parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 2);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::ReactionRequiresUnknownSpecies);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, TroeDetectsBadReactionComponent)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/troe/bad_reaction_component") + extension;
    auto parsed = parse(file);
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

TEST(ParserBase, TroeDetectsUnknownPhase)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/troe/missing_phase") + extension;
    auto parsed = parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::UnknownPhase);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

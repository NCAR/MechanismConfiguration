#include <mechanism_configuration/development/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidTroeReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/troe/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

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
    EXPECT_EQ(mechanism.reactions.troe[0].reactants[0].species_name, "A");
    EXPECT_EQ(mechanism.reactions.troe[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.troe[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe[0].products[0].species_name, "C");
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
    EXPECT_EQ(mechanism.reactions.troe[1].reactants[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.troe[1].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.troe[1].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.troe[1].products[0].species_name, "A");
    EXPECT_EQ(mechanism.reactions.troe[1].products[0].coefficient, 0.2);
    EXPECT_EQ(mechanism.reactions.troe[1].products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe[1].products[0].unknown_properties["__optional thing"], "hello");
    EXPECT_EQ(mechanism.reactions.troe[1].products[1].species_name, "B");
    EXPECT_EQ(mechanism.reactions.troe[1].products[1].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.troe[1].products[1].unknown_properties.size(), 0);
  }
}

TEST(ParserBase, TroeDetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/troe/unknown_species") + extension;
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

TEST(ParserBase, TroeDetectsBadReactionComponent)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/troe/bad_reaction_component") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::InvalidKey);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, TroeDetectsUnknownPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/troe/missing_phase") + extension;
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
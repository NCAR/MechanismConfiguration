#include <mechanism_configuration/development/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidUserDefinedReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/user_defined/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.user_defined.size(), 2);

    EXPECT_EQ(mechanism.reactions.user_defined[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.user_defined[0].name, "my user defined");
    EXPECT_EQ(mechanism.reactions.user_defined[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.user_defined[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].reactants[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.user_defined[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.user_defined[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].unknown_properties["__comment"], "hi");

    EXPECT_EQ(mechanism.reactions.user_defined[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.user_defined[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants.size(), 2);
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants[1].species_name, "A");
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants[1].coefficient, 0.5);
    EXPECT_EQ(mechanism.reactions.user_defined[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined[1].products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.user_defined[1].products[0].coefficient, 0.2);
  }
}

TEST(ParserBase, UserDefinedDetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/user_defined/unknown_species") + extension;
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

TEST(ParserBase, UserDefinedDetectsBadReactionComponent)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/user_defined/bad_reaction_component") + extension;
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

TEST(ParserBase, UserDefinedDetectsUnknownPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/user_defined/missing_phase") + extension;
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

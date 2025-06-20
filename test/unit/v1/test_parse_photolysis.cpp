#include <mechanism_configuration/v1/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidPhotolysisReaction)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/photolysis/valid") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.photolysis.size(), 2);

    EXPECT_EQ(mechanism.reactions.photolysis[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.photolysis[0].name, "my photolysis");
    EXPECT_EQ(mechanism.reactions.photolysis[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.photolysis[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].reactants[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.photolysis[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.photolysis[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].unknown_properties["__comment"], "hi");

    EXPECT_EQ(mechanism.reactions.photolysis[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.photolysis[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.photolysis[1].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[1].reactants[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.photolysis[1].reactants[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.photolysis[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[1].products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.photolysis[1].products[0].coefficient, 0.2);
  }
}

TEST(ParserBase, PhotolysisDetectsUnknownSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/photolysis/unknown_species") + extension;
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

TEST(ParserBase, PhotolysisDetectsBadReactionComponent)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/photolysis/bad_reaction_component") + extension;
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

TEST(ParserBase, PhotolysisDetectsUnknownPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/photolysis/missing_phase") + extension;
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

TEST(ParserBase, PhotolysisDoesNotAcceptMoreThanOneReactant)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/photolysis/more_than_one_reactant") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::TooManyReactionComponents);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}
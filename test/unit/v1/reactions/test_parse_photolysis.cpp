#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidPhotolysisReaction)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = Parse(std::string("v1_unit_configs/reactions/photolysis/valid") + extension);
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.photolysis.size(), 2);

    EXPECT_EQ(mechanism.reactions.photolysis[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.photolysis[0].name, "my photolysis");
    EXPECT_EQ(mechanism.reactions.photolysis[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.photolysis[0].reactants.name, "B");
    EXPECT_EQ(mechanism.reactions.photolysis[0].reactants.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.photolysis[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].unknown_properties["__comment"], "hi");

    EXPECT_EQ(mechanism.reactions.photolysis[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.photolysis[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.photolysis[1].reactants.name, "B");
    EXPECT_EQ(mechanism.reactions.photolysis[1].reactants.coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.photolysis[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[1].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.photolysis[1].products[0].coefficient, 0.2);
  }
}

TEST(ParserBase, PhotolysisDetectsUnknownSpecies)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/photolysis/unknown_species") + extension;
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

TEST(ParserBase, PhotolysisDetectsBadReactionComponent)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/photolysis/bad_reaction_component") + extension;
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

TEST(ParserBase, PhotolysisDetectsUnknownPhase)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/photolysis/missing_phase") + extension;
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

TEST(ParserBase, PhotolysisDoesNotAcceptMoreThanOneReactant)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/photolysis/more_than_one_reactant") + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::TooManyReactionComponents);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

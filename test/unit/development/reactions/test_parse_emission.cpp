#include <mechanism_configuration/development/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidEmissionReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/emission/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.emission.size(), 2);

    EXPECT_EQ(mechanism.reactions.emission[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.emission[0].name, "my emission");
    EXPECT_EQ(mechanism.reactions.emission[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.emission[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.emission[0].products[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.emission[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.emission[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.emission[0].unknown_properties["__comment"], "Dr. Pepper outranks any other soda");

    EXPECT_EQ(mechanism.reactions.emission[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.emission[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.emission[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.emission[1].products[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.emission[1].products[0].coefficient, 1);
  }
}

TEST(ParserBase, EmissionDetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/emission/unknown_species") + extension;
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

TEST(ParserBase, EmissionDetectsBadReactionComponent)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/emission/bad_reaction_component") + extension;
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

TEST(ParserBase, EmissionDetectsUnknownPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/emission/missing_phase") + extension;
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
#include <mechanism_configuration/v1/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidTunnelingReaction)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/tunneling/valid") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.tunneling.size(), 2);

    EXPECT_EQ(mechanism.reactions.tunneling[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.tunneling[0].A, 123.45);
    EXPECT_EQ(mechanism.reactions.tunneling[0].B, 1200.0);
    EXPECT_EQ(mechanism.reactions.tunneling[0].C, 1.0e8);
    EXPECT_EQ(mechanism.reactions.tunneling[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling[0].reactants[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.tunneling[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.tunneling[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling[0].products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.tunneling[0].products[0].coefficient, 1);

    EXPECT_EQ(mechanism.reactions.tunneling[1].name, "my tunneling");
    EXPECT_EQ(mechanism.reactions.tunneling[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.tunneling[1].A, 1.0);
    EXPECT_EQ(mechanism.reactions.tunneling[1].B, 0);
    EXPECT_EQ(mechanism.reactions.tunneling[1].C, 0);
    EXPECT_EQ(mechanism.reactions.tunneling[1].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling[1].reactants[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.tunneling[1].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[0].species_name, "A");
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[0].coefficient, 0.2);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[0].unknown_properties["__optional thing"], "hello");
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[1].species_name, "B");
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[1].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[1].unknown_properties.size(), 0);
  }
}

TEST(ParserBase, TunnelingDetectsUnknownSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/tunneling/unknown_species") + extension;
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

TEST(ParserBase, TunnelingDetectsBadReactionComponent)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/tunneling/bad_reaction_component") + extension;
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

TEST(ParserBase, TunnelingDetectsUnknownPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/tunneling/missing_phase") + extension;
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
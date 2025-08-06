#include <mechanism_configuration/v1/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidCondensedPhaseArrheniusReaction)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/condensed_phase_arrhenius/valid") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius.size(), 3);

    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].name, "my arrhenius");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].A, 32.1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].B, -2.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].C, 102.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].D, 63.4);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].E, -1.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].reactants[0].species_name, "A");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].products[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].products[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].products[1].species_name, "C");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].products[1].coefficient, 0.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].unknown_properties["__solver_param"], "0.1");

    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].name, "my arrhenius2");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].A, 3.1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].B, -0.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].C, 12.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].D, 6.4);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].E, -0.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].reactants.size(), 2);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].reactants[0].species_name, "A");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].reactants[0].coefficient, 2);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].reactants[1].species_name, "B");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].reactants[1].coefficient, 0.1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].products[0].coefficient, 0.5);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].products[0].unknown_properties["__optional thing"], "hello");

    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].name, "");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].A, 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].B, 0);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].C, 0);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].D, 300);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].E, 0);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].reactants[0].species_name, "A");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].products[0].coefficient, 1);
  }
}

TEST(ParserBase, CondensedPhaseArrheniusDetectsUnknownSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/condensed_phase_arrhenius/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::PhaseRequiresUnknownSpecies);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, CondensedPhaseArrheniusDetectsMutuallyExclusiveOptions)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/condensed_phase_arrhenius/mutually_exclusive") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::MutuallyExclusiveOption);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, CondensedPhaseArrheniusDetectsBadReactionComponent)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/condensed_phase_arrhenius/bad_reaction_component") + extension;
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

TEST(ParserBase, CondensedPhaseArrheniusDetectsWhenRequestedSpeciesAreNotInAqueousPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/condensed_phase_arrhenius/species_not_in_aqueous_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_EQ(parsed.errors.size(), 2);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::PhaseRequiresUnknownSpecies);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::PhaseRequiresUnknownSpecies);
    EXPECT_FALSE(parsed);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, CondensedPhaseArrheniusDetectsMissingPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/condensed_phase_arrhenius/missing_phase") + extension;
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
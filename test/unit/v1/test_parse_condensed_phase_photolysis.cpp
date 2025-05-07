#include <gtest/gtest.h>

#include <mechanism_configuration/v1/parser.hpp>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidCondensedPhasePhotolysisReaction)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/condensed_phase_photolysis/valid") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis.size(), 2);

    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].aerosol_phase, "aqueous aerosol");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].aerosol_phase_water, "H2O_aq");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].name, "my condensed phase photolysis");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].reactants[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].unknown_properties["__comment"], "hi");

    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].aerosol_phase, "aqueous aerosol");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].aerosol_phase_water, "H2O_aq");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].reactants[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].reactants[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].products[0].coefficient, 0.2);
  }
}

TEST(ParserBase, CondensedPhasePhotolysisDetectsUnknownSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/condensed_phase_photolysis/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, CondensedPhasePhotolysisDetectsBadReactionComponent)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/condensed_phase_photolysis/bad_reaction_component") + extension;
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

TEST(ParserBase, CondensedPhasePhotolysisDetectsUnknownPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/condensed_phase_photolysis/missing_phase") + extension;
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

TEST(ParserBase, CondensedPhasePhotolysisDoesNotAcceptMoreThanOneReactant)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/condensed_phase_photolysis/more_than_one_reactant") + extension;
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

TEST(ParserBase, CondensedPhasePhotolysisDetectsWhenRequestedSpeciesAreNotInAerosolPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/condensed_phase_photolysis/species_not_in_aerosol_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, CondensedPhasePhotolysisDetectsUnknownAerosolPhaseWater)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/condensed_phase_photolysis/missing_aerosol_phase_water") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}
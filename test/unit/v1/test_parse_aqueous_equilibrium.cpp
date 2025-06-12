#include <gtest/gtest.h>

#include <mechanism_configuration/v1/parser.hpp>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidAqueousEquilibriumReaction)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/aqueous_equilibrium/valid") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium.size(), 2);

    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].name, "my aqueous eq");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].aerosol_phase, "aerosol");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].A, 1.14e-2);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].C, 2300.0);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].k_reverse, 0.32);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].reactants[0].species_name, "A");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].reactants[0].coefficient, 2);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products[1].species_name, "C");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products[1].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].unknown_properties["__comment"], "GIF is pronounced with a hard g");

    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].aerosol_phase, "aerosol");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].A, 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].C, 0);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].k_reverse, 0.32);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].reactants[0].species_name, "A");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].reactants[0].coefficient, 2);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products[1].species_name, "C");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products[1].coefficient, 1);
  }
}

TEST(ParserBase, AqueousEquilibriumDetectsUnknownSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/aqueous_equilibrium/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << std::endl;
    }
  }
}

TEST(ParserBase, AqueousEquilibriumDetectsBadReactionComponent)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/aqueous_equilibrium/bad_reaction_component") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 6);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::InvalidKey);
    EXPECT_EQ(parsed.errors[2].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[3].first, ConfigParseStatus::InvalidKey);
    EXPECT_EQ(parsed.errors[4].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[5].first, ConfigParseStatus::InvalidKey);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, AqueousEquilibriumDetectsUnknownPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/aqueous_equilibrium/missing_phase") + extension;
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
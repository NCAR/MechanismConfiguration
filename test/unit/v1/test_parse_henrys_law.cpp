#include <mechanism_configuration/v1/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidHenrysLawReaction)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/henrys_law/valid") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.henrys_law.size(), 2);

    EXPECT_EQ(mechanism.reactions.henrys_law[0].name, "my henry's law");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].gas.name, "gas");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].gas.species.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].gas.species[0], "A");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solutes.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solutes[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solutes[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solvent.species_name, "H2O");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solvent.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].unknown_properties["__comment"], "B aqueous phase production (kg/m2/s)");

    EXPECT_EQ(mechanism.reactions.henrys_law[1].name, "");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].gas.name, "gas");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].gas.species.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].gas.species[0], "A");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes.size(), 2);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes[1].species_name, "C");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes[1].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solvent.species_name, "H2O");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solvent.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].unknown_properties.size(), 0);
  }
}

TEST(ParserBase, HenrysLawDetectsUnknownSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/henrys_law/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, HenrysLawDetectsGasSpeciesInReactionNotFoundInGasPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/henrys_law/species_not_found_in_gas_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, HenrysLawDetectsUnknownPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/henrys_law/missing_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 3);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::UnknownPhase);
    EXPECT_EQ(parsed.errors[2].first, ConfigParseStatus::UnknownPhase);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, HenrysLawDetectsWhenRequestedSpeciesAreNotInAqueousPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/henrys_law/species_not_in_aqueous_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, HenrysLawDetectsWhenRequestedSolventIsNotRegisteredInCorrectPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/henrys_law/solvent_species_not_registered_in_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}
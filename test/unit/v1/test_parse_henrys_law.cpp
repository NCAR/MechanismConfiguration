#include <gtest/gtest.h>

#include <mechanism_configuration/v1/parser.hpp>

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
    EXPECT_EQ(mechanism.reactions.henrys_law[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].gas_phase_species.species_name, "A");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].gas_phase_species.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].aerosol_phase, "aqueous aerosol");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].aerosol_phase_species.species_name, "B");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].aerosol_phase_species.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].aerosol_phase_water, "H2O_aq");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].unknown_properties["__comment"], "hi");

    EXPECT_EQ(mechanism.reactions.henrys_law[1].name, "");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].gas_phase_species.species_name, "A");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].gas_phase_species.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].aerosol_phase, "aqueous aerosol");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].aerosol_phase_species.species_name, "B");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].aerosol_phase_species.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].aerosol_phase_water, "H2O_aq");
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
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
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

TEST(ParserBase, HenrysLawDetectsUnknownAerosolPhaseWater)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/henrys_law/missing_aerosol_phase_water") + extension;
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

TEST(ParserBase, HenrysLawDetectsWhenRequestedSpeciesAreNotInAerosolPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/henrys_law/species_not_in_aerosol_phase") + extension;
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
#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidHenrysLawReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json"};
  // std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/henrys_law/valid") + extension);
    std::cout << " 0 " << std::endl;

        for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.henrys_law.size(), 2);

    EXPECT_EQ(mechanism.reactions.henrys_law[0].name, "my henry's law");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].gas.name, "gas");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].gas.species.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].gas.species[0].name, "A");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solutes.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solutes[0].name, "B");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solutes[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solvent.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solvent[0].name, "H2O");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solvent[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].unknown_properties["__comment"], "B condensed phase production (kg/m2/s)");

    EXPECT_EQ(mechanism.reactions.henrys_law[1].name, "");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].gas.name, "gas");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].gas.species.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].gas.species[0].name, "A");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes.size(), 2);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes[0].name, "B");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes[1].name, "C");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes[1].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solvent.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solvent[0].name, "H2O");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solvent[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].unknown_properties.size(), 0);
  }
}

TEST(ParserBase, HenrysLawDetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/henrys_law/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::PhaseRequiresUnknownSpecies };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, HenrysLawDetectsGasSpeciesInReactionNotFoundInGasPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        std::string("development_unit_configs/reactions/henrys_law/species_not_found_in_gas_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, HenrysLawDetectsWhenRequestedSpeciesAreNotInAqueousPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/henrys_law/species_not_in_aqueous_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, HenrysLawDetectsWhenRequestedSolventIsNotRegisteredInCorrectPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        std::string("development_unit_configs/reactions/henrys_law/solvent_species_not_registered_in_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}
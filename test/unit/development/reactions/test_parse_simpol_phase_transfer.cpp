#include <mechanism_configuration/development/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidSimpolPhaseTransferReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/simpol_phase_transfer/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer.size(), 2);

    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].name, "my simpol");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].gas_phase_species.species_name, "A");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].gas_phase_species.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].condensed_phase_species.species_name, "B");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].condensed_phase_species.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].B[0], -1.97e3);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].B[1], 2.91e0);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].B[2], 1.96e-3);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].B[3], -4.96e-1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].unknown_properties["__comment"], "cereal is also soup");

    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].name, "");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].gas_phase_species.species_name, "A");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].gas_phase_species.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].condensed_phase_species.species_name, "B");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].condensed_phase_species.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].B[0], -1.97e3);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].B[1], 2.91e0);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].B[2], 1.96e-3);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].B[3], -4.96e-1);
  }
}

TEST(ParserBase, SimpolPhaseTransferDetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/simpol_phase_transfer/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, SimpolPhaseTransferDetectsUnknownAqueousPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        std::string("development_unit_configs/reactions/simpol_phase_transfer/missing_aqueous_phase") + extension;
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

TEST(ParserBase, SimpolPhaseTransferDetectsUnknownGasPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/simpol_phase_transfer/missing_gas_phase") + extension;
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

TEST(ParserBase, SimpolPhaseTransferDetectsUnknownGasPhaseSpeciesNotInGasPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        std::string("development_unit_configs/reactions/simpol_phase_transfer/missing_gas_phase_species_in_gas_phase") +
        extension;
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

TEST(ParserBase, SimpolPhaseTransferDetectsUnknownAqueousPhaseSpeciesNotInAqueousPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        std::string(
            "development_unit_configs/reactions/simpol_phase_transfer/missing_aqueous_phase_species_in_aqueous_phase") +
        extension;
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
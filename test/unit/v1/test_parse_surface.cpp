#include <mechanism_configuration/v1/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidSurfaceReaction)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/surface/valid") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.surface.size(), 2);

    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.surface[0].name, "my surface");
    EXPECT_EQ(mechanism.reactions.surface[0].aqueous_phase, "surface reacting phase");
    EXPECT_EQ(mechanism.reactions.surface[0].reaction_probability, 2.0e-2);
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_species.species_name, "A");
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_species.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products.size(), 2);
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products[1].species_name, "C");
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products[1].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.surface[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.surface[0].unknown_properties["__comment"], "key lime pie is superior to all other pies");

    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.surface[1].aqueous_phase, "surface reacting phase");
    EXPECT_EQ(mechanism.reactions.surface[1].reaction_probability, 1.0);
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_species.species_name, "A");
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_species.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products.size(), 2);
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[0].unknown_properties["__optional thing"], "hello");
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[1].species_name, "C");
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[1].coefficient, 1);
  }
}

TEST(ParserBase, SurfaceDetectsUnknownSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/surface/unknown_species") + extension;
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

TEST(ParserBase, SurfaceDetectsBadReactionComponent)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/surface/bad_reaction_component") + extension;
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

TEST(ParserBase, SurfaceDetectsUnknownAqueousPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/surface/missing_aqueous_phase") + extension;
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

TEST(ParserBase, SurfaceDetectsUnknownGasPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/surface/missing_gas_phase") + extension;
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
#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidPhotolysisReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/photolysis/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.photolysis.size(), 2);

    EXPECT_EQ(mechanism.reactions.photolysis[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.photolysis[0].name, "my photolysis");
    EXPECT_EQ(mechanism.reactions.photolysis[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.photolysis[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].reactants[0].name, "B");
    EXPECT_EQ(mechanism.reactions.photolysis[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.photolysis[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[0].unknown_properties["__comment"], "hi");

    EXPECT_EQ(mechanism.reactions.photolysis[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.photolysis[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.photolysis[1].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[1].reactants[0].name, "B");
    EXPECT_EQ(mechanism.reactions.photolysis[1].reactants[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.photolysis[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis[1].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.photolysis[1].products[0].coefficient, 0.2);
  }
}

TEST(ParserBase, PhotolysisDetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/photolysis/unknown_species") + extension;
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

TEST(ParserBase, PhotolysisDetectsBadReactionComponent)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/photolysis/bad_reaction_component") + extension;
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

TEST(ParserBase, PhotolysisDetectsUnknownPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/photolysis/missing_phase") + extension;
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

TEST(ParserBase, PhotolysisDoesNotAcceptMoreThanOneReactant)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/photolysis/more_than_one_reactant") + extension;
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

TEST(ParserBase, PhotolysisInvalidNumberReactantUnknownSpeciesUnknownPhaseFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" }, types::Species{ .name = "bar" } };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "PHOTOLYSIS";
  reaction_node["products"] = YAML::Load("[{ name: foo }]");

  // Invalid number of reactions triggers validation error
  reaction_node["reactants"] = YAML::Load("[{ name: quiz }, { name: bar }]");

  // Unknown gas phase name triggers validation error
  reaction_node["gas phase"] = "what is first order loss phase";

  PhotolysisParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  ASSERT_FALSE(errors.empty());
  EXPECT_EQ(errors.size(), 3);
  EXPECT_EQ(errors[0].first, ConfigParseStatus::TooManyReactionComponents);
  EXPECT_EQ(errors[1].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
  EXPECT_EQ(errors[2].first, ConfigParseStatus::UnknownPhase);
  for (auto& error : errors)
  {
    std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
  }
}
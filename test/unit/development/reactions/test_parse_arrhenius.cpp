#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidArrheniusReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/arrhenius/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.arrhenius.size(), 3);

    EXPECT_EQ(mechanism.reactions.arrhenius[0].name, "my arrhenius");
    EXPECT_EQ(mechanism.reactions.arrhenius[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.arrhenius[0].A, 32.1);
    EXPECT_EQ(mechanism.reactions.arrhenius[0].B, -2.3);
    EXPECT_EQ(mechanism.reactions.arrhenius[0].C, 102.3);
    EXPECT_EQ(mechanism.reactions.arrhenius[0].D, 63.4);
    EXPECT_EQ(mechanism.reactions.arrhenius[0].E, -1.3);
    EXPECT_EQ(mechanism.reactions.arrhenius[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.arrhenius[0].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.arrhenius[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.arrhenius[0].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.arrhenius[0].products[0].name, "B");
    EXPECT_EQ(mechanism.reactions.arrhenius[0].products[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.arrhenius[0].products[1].name, "C");
    EXPECT_EQ(mechanism.reactions.arrhenius[0].products[1].coefficient, 0.3);
    EXPECT_EQ(mechanism.reactions.arrhenius[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.arrhenius[0].unknown_properties["__solver_param"], "0.1");

    EXPECT_EQ(mechanism.reactions.arrhenius[1].name, "my arrhenius2");
    EXPECT_EQ(mechanism.reactions.arrhenius[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.arrhenius[1].A, 3.1);
    EXPECT_EQ(mechanism.reactions.arrhenius[1].B, -0.3);
    EXPECT_EQ(mechanism.reactions.arrhenius[1].C, 12.3);
    EXPECT_EQ(mechanism.reactions.arrhenius[1].D, 6.4);
    EXPECT_EQ(mechanism.reactions.arrhenius[1].E, -0.3);
    EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants.size(), 2);
    EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants[0].coefficient, 2);
    EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants[1].name, "B");
    EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants[1].coefficient, 0.1);
    EXPECT_EQ(mechanism.reactions.arrhenius[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.arrhenius[1].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.arrhenius[1].products[0].coefficient, 0.5);
    EXPECT_EQ(mechanism.reactions.arrhenius[1].products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.arrhenius[1].products[0].unknown_properties["__optional thing"], "hello");

    EXPECT_EQ(mechanism.reactions.arrhenius[2].name, "");
    EXPECT_EQ(mechanism.reactions.arrhenius[2].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.arrhenius[2].A, 1);
    EXPECT_EQ(mechanism.reactions.arrhenius[2].B, 0);
    EXPECT_EQ(mechanism.reactions.arrhenius[2].C, 0);
    EXPECT_EQ(mechanism.reactions.arrhenius[2].D, 300);
    EXPECT_EQ(mechanism.reactions.arrhenius[2].E, 0);
    EXPECT_EQ(mechanism.reactions.arrhenius[2].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.arrhenius[2].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.arrhenius[2].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.arrhenius[2].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.arrhenius[2].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.arrhenius[2].products[0].coefficient, 1);
  }
}

TEST(ParserBase, ArrheniusDetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/arrhenius/unknown_species") + extension;
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

TEST(ArrheniusParserTest, MutuallyExclusiveEaAndCFailsValidation)
{
  using namespace development;

  YAML::Node reaction_node;
  reaction_node["reactants"] = YAML::Load("[{ name: foo }]");
  reaction_node["products"] = YAML::Load("[{ name: bar }]");
  reaction_node["type"] = "Arrhenius";
  reaction_node["gas phase"] = "gas";

  // Specify both Ea and C to trigger validation error
  reaction_node["Ea"] = 0.5;
  reaction_node["C"] = 10.0;

  std::vector<types::Species> existing_species = {
      types::Species{ .name = "foo" },
      types::Species{ .name = "bar" }
  };
  std::vector<types::Phase> existing_phases = {
      types::Phase{ .name = "gas" }
  };

  ArrheniusParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  ASSERT_FALSE(errors.empty());
  EXPECT_EQ(errors[0].first, ConfigParseStatus::MutuallyExclusiveOption);
  for (auto& error : errors)
  {
    std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
  }
}

TEST(ParserBase, ArrheniusDetectsMutuallyExclusiveOptions)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/arrhenius/mutually_exclusive") + extension;
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

TEST(ParserBase, ArrheniusDetectsBadReactionComponent)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/arrhenius/bad_reaction_component") + extension;
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

TEST(ParserBase, ArrheniusDetectsUnknownPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/arrhenius/missing_phase") + extension;
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
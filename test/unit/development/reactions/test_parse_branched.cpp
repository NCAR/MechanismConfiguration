#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidBranchedReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/branched/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.branched.size(), 1);

    EXPECT_EQ(mechanism.reactions.branched[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.branched[0].name, "my branched");
    EXPECT_EQ(mechanism.reactions.branched[0].X, 1.2e-4);
    EXPECT_EQ(mechanism.reactions.branched[0].Y, 167);
    EXPECT_EQ(mechanism.reactions.branched[0].a0, 0.15);
    EXPECT_EQ(mechanism.reactions.branched[0].n, 9);
    EXPECT_EQ(mechanism.reactions.branched[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched[0].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.branched[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products[0].unknown_properties["__thing"], "hi");
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products.size(), 2);
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products[0].name, "B");
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products[0].coefficient, 0.2);
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products[1].name, "A");
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products[1].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.branched[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched[0].unknown_properties["__comment"], "thing");
  }
}

TEST(ParserBase, BranchedDetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/branched/unknown_species") + extension;
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

TEST(ParserBase, BranchedDetectsBadReactionComponent)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/branched/bad_reaction_component") + extension;
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

TEST(ParserBase, BranchedDetectsUnknownPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/branched/missing_phase") + extension;
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

TEST(ParserBase, BranchedMissingRequiredKeyFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" },
                                                   types::Species{ .name = "bar" },
                                                   types::Species{ .name = "quiz" } };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "BRANCHED_NO_RO2";
  reaction_node["gas phase"] = "gas";
  reaction_node["reactants"] = YAML::Load("[{ name: foo }]");
  reaction_node["alkoxy products"] = YAML::Load("[{ name: bar }]");

  // Incorrect required key triggers validation error
  reaction_node["wrong nitrate products"] = YAML::Load("[{ name: quiz }]");

  BranchedParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  ASSERT_FALSE(errors.empty());
  EXPECT_EQ(errors.size(), 2);
  EXPECT_EQ(errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
  EXPECT_EQ(errors[1].first, ConfigParseStatus::InvalidKey);
  for (auto& error : errors)
  {
    std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
  }
}

TEST(ParserBase, BranchedUnknownSpeciesAndUnknownPhaseFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" },
                                                   types::Species{ .name = "bar" },
                                                   types::Species{ .name = "quiz" } };

  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "BRANCHED_NO_RO2";
  reaction_node["reactants"] = YAML::Load("[{ name: foo }]");
  reaction_node["nitrate products"] = YAML::Load("[{ name: quiz }]");

  // Unknown species triggers validation error
  reaction_node["alkoxy products"] = YAML::Load("[{ name: bar }, { name: ABC }]");

  // Unknown gas phase name triggers validation error
  reaction_node["gas phase"] = "Gaseous Phase";

  BranchedParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  ASSERT_FALSE(errors.empty());
  EXPECT_EQ(errors.size(), 2);
  EXPECT_EQ(errors[0].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
  EXPECT_EQ(errors[1].first, ConfigParseStatus::UnknownPhase);
  for (auto& error : errors)
  {
    std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
  }
}
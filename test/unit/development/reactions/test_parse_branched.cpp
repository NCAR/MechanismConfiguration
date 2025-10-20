#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>
#include <set>

#include <set>

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

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::ReactionRequiresUnknownSpecies };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
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

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound, ConfigParseStatus::InvalidKey };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
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

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::UnknownPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
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
  EXPECT_EQ(errors.size(), 2);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound, ConfigParseStatus::InvalidKey };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
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
  EXPECT_EQ(errors.size(), 2);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::ReactionRequiresUnknownSpecies,
                                                ConfigParseStatus::UnknownPhase };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}
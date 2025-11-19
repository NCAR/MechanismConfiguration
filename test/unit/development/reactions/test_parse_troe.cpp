#include <mechanism_configuration/development/mechanism_parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParserTroe, ParseValidConfig)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/troe/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.troe.size(), 2);

    EXPECT_EQ(mechanism.reactions.troe[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.troe[0].k0_A, 1.0);
    EXPECT_EQ(mechanism.reactions.troe[0].k0_B, 0.0);
    EXPECT_EQ(mechanism.reactions.troe[0].k0_C, 0.0);
    EXPECT_EQ(mechanism.reactions.troe[0].kinf_A, 1.0);
    EXPECT_EQ(mechanism.reactions.troe[0].kinf_B, 0.0);
    EXPECT_EQ(mechanism.reactions.troe[0].kinf_C, 0.0);
    EXPECT_EQ(mechanism.reactions.troe[0].Fc, 0.6);
    EXPECT_EQ(mechanism.reactions.troe[0].N, 1.0);
    EXPECT_EQ(mechanism.reactions.troe[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe[0].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.troe[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.troe[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe[0].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.troe[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.troe[0].unknown_properties.size(), 1);
    if (extension == ".json")
    {
      EXPECT_EQ(mechanism.reactions.troe[0].unknown_properties["__my object"], "{a: 1.0}");
    }
    else
    {
      EXPECT_EQ(mechanism.reactions.troe[0].unknown_properties["__my object"], "a: 1.0");
    }

    EXPECT_EQ(mechanism.reactions.troe[1].name, "my troe");
    EXPECT_EQ(mechanism.reactions.troe[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.troe[1].k0_A, 32.1);
    EXPECT_EQ(mechanism.reactions.troe[1].k0_B, -2.3);
    EXPECT_EQ(mechanism.reactions.troe[1].k0_C, 102.3);
    EXPECT_EQ(mechanism.reactions.troe[1].kinf_A, 63.4);
    EXPECT_EQ(mechanism.reactions.troe[1].kinf_B, -1.3);
    EXPECT_EQ(mechanism.reactions.troe[1].kinf_C, 908.5);
    EXPECT_EQ(mechanism.reactions.troe[1].Fc, 1.3);
    EXPECT_EQ(mechanism.reactions.troe[1].N, 32.1);
    EXPECT_EQ(mechanism.reactions.troe[1].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe[1].reactants[0].name, "C");
    EXPECT_EQ(mechanism.reactions.troe[1].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.troe[1].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.troe[1].products[0].name, "A");
    EXPECT_EQ(mechanism.reactions.troe[1].products[0].coefficient, 0.2);
    EXPECT_EQ(mechanism.reactions.troe[1].products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe[1].products[0].unknown_properties["__optional thing"], "hello");
    EXPECT_EQ(mechanism.reactions.troe[1].products[1].name, "B");
    EXPECT_EQ(mechanism.reactions.troe[1].products[1].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.troe[1].products[1].unknown_properties.size(), 0);
  }
}

TEST(ParserTroe, DetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/troe/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::ReactionRequiresUnknownSpecies,
                                                  ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserTroe, DetectsBadReactionComponent)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/troe/bad_reaction_component") + extension;
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

TEST(ParserTroe, DetectsUnknownPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/troe/missing_phase") + extension;
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

TEST(ValidateTroe, UnknownSpeciesAndUnknownPhaseFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" },
                                                   types::Species{ .name = "bar" },
                                                   types::Species{ .name = "quiz" } };

  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "TROE";
  reaction_node["products"] = YAML::Load("[{ name: quiz }]");

  // Unknown species triggers validation error
  reaction_node["reactants"] = YAML::Load("[{ name: bar }, { name: ABC }]");

  // Unknown gas phase name triggers validation error
  reaction_node["gas phase"] = "Gaseous Phase";

  TroeParser parser;
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
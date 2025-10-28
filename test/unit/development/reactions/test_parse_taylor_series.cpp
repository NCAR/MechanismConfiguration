#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParseTaylorSeries, ParseValidConfig)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/taylor_series/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.taylor_series.size(), 3);

    EXPECT_EQ(mechanism.reactions.taylor_series[0].name, "my taylor_series");
    EXPECT_EQ(mechanism.reactions.taylor_series[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.taylor_series[0].A, 32.1);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].B, -2.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].C, 102.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].D, 63.4);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].E, -1.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].taylor_coefficients.size(), 3);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].taylor_coefficients[0], 1.0);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].taylor_coefficients[1], 2.0);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].taylor_coefficients[2], 3.0);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.taylor_series[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].products[0].name, "B");
    EXPECT_EQ(mechanism.reactions.taylor_series[0].products[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].products[1].name, "C");
    EXPECT_EQ(mechanism.reactions.taylor_series[0].products[1].coefficient, 0.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].unknown_properties["__solver_param"], "0.1");

    EXPECT_EQ(mechanism.reactions.taylor_series[1].name, "my taylor_series2");
    EXPECT_EQ(mechanism.reactions.taylor_series[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.taylor_series[1].A, 3.1);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].B, -0.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].C, 12.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].D, 6.4);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].E, -0.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].taylor_coefficients.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].taylor_coefficients[0], 10.5);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].reactants.size(), 2);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.taylor_series[1].reactants[0].coefficient, 2);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].reactants[1].name, "B");
    EXPECT_EQ(mechanism.reactions.taylor_series[1].reactants[1].coefficient, 0.1);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.taylor_series[1].products[0].coefficient, 0.5);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].products[0].unknown_properties["__optional thing"], "hello");

    EXPECT_EQ(mechanism.reactions.taylor_series[2].name, "");
    EXPECT_EQ(mechanism.reactions.taylor_series[2].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.taylor_series[2].A, 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].B, 0);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].C, 0);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].D, 300);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].E, 0);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].taylor_coefficients.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].taylor_coefficients[0], 1.0);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.taylor_series[2].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.taylor_series[2].products[0].coefficient, 1);
  }
}

TEST(ParseTaylorSeries, DetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/taylor_series/unknown_species") + extension;
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

TEST(ParseTaylorSeries, DetectsMutuallyExclusiveOptions)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/taylor_series/mutually_exclusive") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::MutuallyExclusiveOption };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParseTaylorSeries, DetectsBadReactionComponent)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/taylor_series/bad_reaction_component") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::InvalidKey, ConfigParseStatus::RequiredKeyNotFound };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParseTaylorSeries, DetectsUnknownPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/taylor_series/missing_phase") + extension;
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

TEST(ValidateTaylorSeries, MutuallyExclusiveEaAndCFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" }, types::Species{ .name = "bar" } };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } };

  YAML::Node reaction_node;
  reaction_node["reactants"] = YAML::Load("[{ name: foo }]");
  reaction_node["products"] = YAML::Load("[{ name: bar }]");
  reaction_node["type"] = "TAYLOR_SERIES";
  reaction_node["gas phase"] = "gas";

  // Specify both Ea and C to trigger validation error
  reaction_node["Ea"] = 0.5;
  reaction_node["C"] = 10.0;

  TaylorSeriesParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 1);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::MutuallyExclusiveOption };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

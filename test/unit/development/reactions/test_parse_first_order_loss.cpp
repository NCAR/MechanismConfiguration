#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidFirstOrderLossReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/first_order_loss/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.first_order_loss.size(), 2);

    EXPECT_EQ(mechanism.reactions.first_order_loss[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].name, "my first order loss");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].reactants.name, "C");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].reactants.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].unknown_properties.size(), 1);
    EXPECT_EQ(
        mechanism.reactions.first_order_loss[0].unknown_properties["__comment"], "Strawberries are the superior fruit");

    EXPECT_EQ(mechanism.reactions.first_order_loss[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].reactants.name, "C");
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].reactants.coefficient, 1);
  }
}

TEST(ParserBase, FirstOrderLossDetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/first_order_loss/unknown_species") + extension;
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

TEST(ParserBase, FirstOrderLossDetectsBadReactionComponent)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/first_order_loss/bad_reaction_component") + extension;
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

TEST(ParserBase, FirstOrderLossDetectsUnknownPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/first_order_loss/missing_phase") + extension;
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

TEST(ParserBase, FirstOrderLossDetectsMoreThanOneSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/first_order_loss/too_many_reactants") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::TooManyReactionComponents };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, FirstOrderLossInvalidNumberReactantUnknownSpeciesUnknownPhaseFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" }, types::Species{ .name = "bar" } };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "FIRST_ORDER_LOSS";

  // Invalid number of reactions triggers validation error
  reaction_node["reactants"] = YAML::Load("[{ name: quiz }, { name: bar }]");

  // Unknown gas phase name triggers validation error
  reaction_node["gas phase"] = "what is first order loss phase";

  FirstOrderLossParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 3);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::TooManyReactionComponents,
                                                ConfigParseStatus::ReactionRequiresUnknownSpecies,
                                                ConfigParseStatus::UnknownPhase };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}
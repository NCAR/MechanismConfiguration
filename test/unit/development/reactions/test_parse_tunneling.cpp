#include <mechanism_configuration/development/mechanism.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParseTunneling, ParseValidConfig)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/tunneling/valid";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 0) << "Validation errors were: " << validation_errors.size();

    auto mechanism = parser.Parse(object);
    EXPECT_EQ(mechanism.reactions.tunneling.size(), 2);

    EXPECT_EQ(mechanism.reactions.tunneling[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.tunneling[0].A, 123.45);
    EXPECT_EQ(mechanism.reactions.tunneling[0].B, 1200.0);
    EXPECT_EQ(mechanism.reactions.tunneling[0].C, 1.0e8);
    EXPECT_EQ(mechanism.reactions.tunneling[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling[0].reactants[0].name, "B");
    EXPECT_EQ(mechanism.reactions.tunneling[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.tunneling[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling[0].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.tunneling[0].products[0].coefficient, 1);

    EXPECT_EQ(mechanism.reactions.tunneling[1].name, "my tunneling");
    EXPECT_EQ(mechanism.reactions.tunneling[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.tunneling[1].A, 1.0);
    EXPECT_EQ(mechanism.reactions.tunneling[1].B, 0);
    EXPECT_EQ(mechanism.reactions.tunneling[1].C, 0);
    EXPECT_EQ(mechanism.reactions.tunneling[1].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling[1].reactants[0].name, "B");
    EXPECT_EQ(mechanism.reactions.tunneling[1].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[0].name, "A");
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[0].coefficient, 0.2);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[0].unknown_properties["__optional thing"], "hello");
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[1].name, "B");
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[1].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[1].unknown_properties.size(), 0);
  }
}

TEST(ParseTunneling, DetectsUnknownSpecies)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/tunneling/unknown_species";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 2);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::ReactionRequiresUnknownSpecies,
                                                  ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParseTunneling, DetectsBadReactionComponent)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/tunneling/bad_reaction_component";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 2);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound, ConfigParseStatus::InvalidKey };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParseTunneling, DetectsUnknownPhase)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/tunneling/missing_phase";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::UnknownPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ValidateTunneling, UnknownSpeciesAndUnknownPhaseFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" },
                                                   types::Species{ .name = "bar" },
                                                   types::Species{ .name = "quiz" } };

  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "TUNNELING";
  reaction_node["products"] = YAML::Load("[{ name: quiz }]");

  // Unknown species triggers validation error
  reaction_node["reactants"] = YAML::Load("[{ name: bar }, { name: ABC }]");

  // Unknown gas phase name triggers validation error
  reaction_node["gas phase"] = "Gaseous Phase";

  TunnelingParser parser;
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
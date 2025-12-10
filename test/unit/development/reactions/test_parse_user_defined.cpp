#include <mechanism_configuration/development/mechanism.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParseUserDefined, ParseValidConfig)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/user_defined/valid";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 0) << "Validation errors were: " << validation_errors.size();

    auto mechanism = parser.Parse(object);
    EXPECT_EQ(mechanism.reactions.user_defined.size(), 2);

    EXPECT_EQ(mechanism.reactions.user_defined[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.user_defined[0].name, "my user defined");
    EXPECT_EQ(mechanism.reactions.user_defined[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.user_defined[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].reactants[0].name, "B");
    EXPECT_EQ(mechanism.reactions.user_defined[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.user_defined[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined[0].unknown_properties["__comment"], "hi");

    EXPECT_EQ(mechanism.reactions.user_defined[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.user_defined[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants.size(), 2);
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants[0].name, "B");
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants[1].name, "A");
    EXPECT_EQ(mechanism.reactions.user_defined[1].reactants[1].coefficient, 0.5);
    EXPECT_EQ(mechanism.reactions.user_defined[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined[1].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.user_defined[1].products[0].coefficient, 0.2);
  }
}

TEST(ParseUserDefined, DetectsUnknownSpecies)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/user_defined/unknown_species";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 2);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::ReactionRequiresUnknownSpecies,
                                                  ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase };
    ;
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParseUserDefined, DetectsBadReactionComponent)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/user_defined/bad_reaction_component";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 3);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::InvalidKey,
                                                  ConfigParseStatus::InvalidKey,
                                                  ConfigParseStatus::RequiredKeyNotFound };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParseUserDefined, DetectsUnknownPhase)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/user_defined/missing_phase";
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

TEST(ValidateUserDefined, UnknownSpeciesAndUnknownPhaseFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" },
                                                   types::Species{ .name = "bar" },
                                                   types::Species{ .name = "quiz" } };

  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "USER_DEFINED";
  reaction_node["products"] = YAML::Load("[{ name: quiz }]");

  // Unknown species triggers validation error
  reaction_node["reactants"] = YAML::Load("[{ name: bar }, { name: ABC }]");

  // Unknown gas phase name triggers validation error
  reaction_node["gas phase"] = "Gaseous Phase";

  UserDefinedParser parser;
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
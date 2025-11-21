#include <mechanism_configuration/development/mechanism.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParseCondensedPhaseArrhenius, ParseValidConfig)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/condensed_phase_arrhenius/valid";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 0) << "Validation errors were: " << validation_errors.size();

    auto mechanism = parser.Parse(object);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius.size(), 3);

    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].name, "my arrhenius");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].A, 32.1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].B, -2.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].C, 102.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].D, 63.4);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].E, -1.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].products[0].name, "B");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].products[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].products[1].name, "C");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].products[1].coefficient, 0.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[0].unknown_properties["__solver_param"], "0.1");

    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].name, "my arrhenius2");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].A, 3.1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].B, -0.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].C, 12.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].D, 6.4);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].E, -0.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].reactants.size(), 2);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].reactants[0].coefficient, 2);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].reactants[1].name, "B");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].reactants[1].coefficient, 0.1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].products[0].coefficient, 0.5);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[1].products[0].unknown_properties["__optional thing"], "hello");

    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].name, "");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].A, 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].B, 0);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].C, 0);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].D, 300);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].E, 0);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius[2].products[0].coefficient, 1);
  }
}

TEST(ParseCondensedPhaseArrhenius, DetectsUnknownSpecies)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/condensed_phase_arrhenius/unknown_species";
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

TEST(ParseCondensedPhaseArrhenius, DetectsMutuallyExclusiveOptions)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/condensed_phase_arrhenius/mutually_exclusive";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::MutuallyExclusiveOption };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParseCondensedPhaseArrhenius, DetectsBadReactionComponent)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/condensed_phase_arrhenius/bad_reaction_component";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 2);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::InvalidKey, ConfigParseStatus::RequiredKeyNotFound };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParseCondensedPhaseArrhenius, DetectsWhenRequestedSpeciesAreNotInAqueousPhase)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/condensed_phase_arrhenius/species_not_in_aqueous_phase";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 2);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase,
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

TEST(ParseCondensedPhaseArrhenius, DetectsMissingPhase)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/condensed_phase_arrhenius/missing_phase";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 1);;

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

TEST(ValidateCondensedPhaseArrhenius, MutuallyExclusiveEaAndCFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" }, types::Species{ .name = "bar" } };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "aqueous" } };

  YAML::Node reaction_node;
  reaction_node["reactants"] = YAML::Load("[{ name: foo }]");
  reaction_node["products"] = YAML::Load("[{ name: bar }]");
  reaction_node["type"] = "CONDENSED_PHASE_ARRHENIUS";
  reaction_node["condensed phase"] = "organic";

  // Specify both Ea and C to trigger validation error
  reaction_node["Ea"] = 0.5;
  reaction_node["C"] = 10.0;

  CondensedPhaseArrheniusParser parser;
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
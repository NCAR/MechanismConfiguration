#include <mechanism_configuration/development/mechanism.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParseCondensedPhasePhotolysis, ParseValidConfig)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/condensed_phase_photolysis/valid";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 0) << "Validation errors were: " << validation_errors.size();

    auto mechanism = parser.Parse(object);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis.size(), 2);

    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].name, "my condensed phase photolysis");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].reactants.name, "B");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].reactants.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].unknown_properties["__comment"], "hi");

    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].reactants.name, "B");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].reactants.coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].products[0].coefficient, 0.2);
  }
}

TEST(ParseCondensedPhasePhotolysis, DetectsUnknownSpecies)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/condensed_phase_photolysis/unknown_species";
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

TEST(ParseCondensedPhasePhotolysis, DetectsBadReactionComponent)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/condensed_phase_photolysis/bad_reaction_component";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::InvalidKey };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParseCondensedPhasePhotolysis, DetectsUnknownPhase)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/condensed_phase_photolysis/missing_phase";
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

TEST(ParseCondensedPhasePhotolysis, DoesNotAcceptMoreThanOneReactant)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/condensed_phase_photolysis/more_than_one_reactant";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::TooManyReactionComponents };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParseCondensedPhasePhotolysis, DetectsWhenRequestedSpeciesAreNotInAqueousPhase)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/condensed_phase_photolysis/species_not_in_aqueous_phase";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ValidateCondensedPhasePhotolysis, InvalidNumberReactantUnknownSpeciesUnknownPhaseFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" }, types::Species{ .name = "bar" } };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "aqueous" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "CONDENSED_PHASE_PHOTOLYSIS";
  reaction_node["products"] = YAML::Load("[{ name: foo }]");

  // Invalid number of reactions triggers validation error
  reaction_node["reactants"] = YAML::Load("[{ name: quiz }, { name: bar }]");

  // Unknown phase name triggers validation error
  reaction_node["condensed phase"] = "organic";

  CondensedPhasePhotolysisParser parser;
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
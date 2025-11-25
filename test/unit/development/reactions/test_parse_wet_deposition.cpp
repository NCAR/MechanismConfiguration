#include <mechanism_configuration/development/mechanism.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParseWetDeposition, ParseValidConfig)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/wet_deposition/valid";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 0) << "Validation errors were: " << validation_errors.size();

    auto mechanism = parser.Parse(object);
    EXPECT_EQ(mechanism.reactions.wet_deposition.size(), 2);

    EXPECT_EQ(mechanism.reactions.wet_deposition[0].name, "rxn cloud");
    EXPECT_EQ(mechanism.reactions.wet_deposition[0].condensed_phase, "cloud");
    EXPECT_EQ(mechanism.reactions.wet_deposition[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.wet_deposition[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.wet_deposition[0].unknown_properties["__comment"], "Tuxedo cats are the best");

    EXPECT_EQ(mechanism.reactions.wet_deposition[1].name, "rxn cloud2");
    EXPECT_EQ(mechanism.reactions.wet_deposition[1].condensed_phase, "cloud");
    EXPECT_EQ(mechanism.reactions.wet_deposition[1].scaling_factor, 1);
  }
}

TEST(ParseWetDeposition, DetectsUnknownPhase)
{
  development::Parser parser;

  std::string path = "development_unit_configs/reactions/wet_deposition/missing_phase";
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

TEST(ValidateWetDeposition, ReturnsEmptyErrorsForValidReaction)
{
  std::vector<development::types::Species> existing_species;
  development::types::Species species1;
  species1.name = "A";
  existing_species.emplace_back(species1);

  std::vector<development::types::Phase> existing_phases;
  development::types::Phase phase1;
  phase1.name = "cloud";
  existing_phases.emplace_back(phase1);

  YAML::Node reaction = YAML::Load(R"(
    type: "WET_DEPOSITION"
    "condensed phase": "cloud"
    name: "rxn cloud"
    "scaling factor": 12.3
  )");

  development::WetDepositionParser parser;
  auto errors = parser.Validate(reaction, existing_species, existing_phases);
  EXPECT_TRUE(errors.empty());
}

TEST(ValidateWetDeposition, DetectsMissingRequiredType)
{
  std::vector<development::types::Species> existing_species;
  std::vector<development::types::Phase> existing_phases;
  development::types::Phase phase1;
  phase1.name = "cloud";
  existing_phases.emplace_back(phase1);

  YAML::Node reaction = YAML::Load(R"(
    "condensed phase": "cloud"
    name: "rxn cloud"
  )");

  development::WetDepositionParser parser;
  auto errors = parser.Validate(reaction, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 1);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

TEST(ValidateWetDeposition, DetectsMissingRequiredCondensedPhase)
{
  std::vector<development::types::Species> existing_species;
  std::vector<development::types::Phase> existing_phases;
  development::types::Phase phase1;
  phase1.name = "cloud";
  existing_phases.emplace_back(phase1);

  YAML::Node reaction = YAML::Load(R"(
    type: "WET_DEPOSITION"
    name: "rxn cloud"
  )");

  development::WetDepositionParser parser;
  auto errors = parser.Validate(reaction, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 1);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

TEST(ValidateWetDeposition, ValidatesWithOptionalScalingFactor)
{
  std::vector<development::types::Species> existing_species;
  std::vector<development::types::Phase> existing_phases;
  development::types::Phase phase1;
  phase1.name = "cloud";
  existing_phases.emplace_back(phase1);

  YAML::Node reaction = YAML::Load(R"(
    type: "WET_DEPOSITION"
    "condensed phase": "cloud"
    "scaling factor": 5.7
  )");

  development::WetDepositionParser parser;
  auto errors = parser.Validate(reaction, existing_species, existing_phases);
  EXPECT_TRUE(errors.empty());
}

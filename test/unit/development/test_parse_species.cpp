#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/validator.hpp>

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParseSpecies, ParseValidConfig)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/species/valid_species") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;
    EXPECT_EQ(mechanism.species.size(), 3);

    EXPECT_EQ(mechanism.species[0].name, "A");
    EXPECT_EQ(mechanism.species[0].unknown_properties.size(), 2);
    EXPECT_EQ(mechanism.species[0].unknown_properties["__absolute tolerance"], "1.0e-30");
    EXPECT_EQ(mechanism.species[0].unknown_properties["__long name"], "ozone");
    EXPECT_EQ(mechanism.species[0].tracer_type.has_value(), false);
    EXPECT_EQ(mechanism.species[0].is_third_body.has_value(), true);
    EXPECT_EQ(mechanism.species[0].is_third_body.value(), true);

    EXPECT_EQ(mechanism.species[1].name, "H2O2");
    EXPECT_EQ(mechanism.species[1].henrys_law_constant_298.has_value(), true);
    EXPECT_EQ(mechanism.species[1].henrys_law_constant_298.value(), 1.011596348);
    EXPECT_EQ(mechanism.species[1].henrys_law_constant_exponential_factor.has_value(), true);
    EXPECT_EQ(mechanism.species[1].henrys_law_constant_exponential_factor.value(), 6340);
    EXPECT_EQ(mechanism.species[1].diffusion_coefficient.has_value(), true);
    EXPECT_EQ(mechanism.species[1].diffusion_coefficient.value(), 1.46e-05);
    EXPECT_EQ(mechanism.species[1].n_star.has_value(), true);
    EXPECT_EQ(mechanism.species[1].n_star.value(), 1.74);
    EXPECT_EQ(mechanism.species[1].molecular_weight.has_value(), true);
    EXPECT_EQ(mechanism.species[1].molecular_weight.value(), 0.0340147);
    EXPECT_EQ(mechanism.species[1].density.has_value(), true);
    EXPECT_EQ(mechanism.species[1].density.value(), 1000.0);
    EXPECT_EQ(mechanism.species[1].constant_concentration.has_value(), true);
    EXPECT_EQ(mechanism.species[1].constant_concentration.value(), 2.5e19);
    EXPECT_EQ(mechanism.species[1].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.species[1].unknown_properties["__absolute tolerance"], "1.0e-10");

    EXPECT_EQ(mechanism.species[2].name, "aerosol stuff");
    EXPECT_EQ(mechanism.species[2].molecular_weight.has_value(), true);
    EXPECT_EQ(mechanism.species[2].molecular_weight.value(), 0.5);
    EXPECT_EQ(mechanism.species[2].density.has_value(), true);
    EXPECT_EQ(mechanism.species[2].density.value(), 1000.0);
    EXPECT_EQ(mechanism.species[2].constant_mixing_ratio.has_value(), true);
    EXPECT_EQ(mechanism.species[2].constant_mixing_ratio.value(), 1.0e-6);
    EXPECT_EQ(mechanism.species[2].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.species[2].unknown_properties["__absolute tolerance"], "1.0e-20");
  }
}

TEST(ParseSpecies, DetectsDuplicateSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/species/duplicate_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 4);

    for (const auto& [status, message] : parsed.errors)
    {
      EXPECT_EQ(status, ConfigParseStatus::DuplicateSpeciesDetected);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
  }
}

TEST(ParseSpecies, DetectsMissingRequiredKeys)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/species/missing_required_key") + extension;
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

TEST(ParseSpecies, DetectsInvalidKeys)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/species/invalid_key") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::InvalidKey };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ValidateSpecies, ReturnsEmptyErrorsForValidSpecies)
{
  YAML::Node species_list = YAML::Load(R"(
    - "name": "A"
      "absolute tolerance": "1.0e-30"
      "is third body": true
    - "name": "B"
      "molecular weight [kg mol-1]": 0.034
      "density [kg m-3]": 1000.0
  )");

  auto errors = development::ValidateSpecies(species_list);
  EXPECT_TRUE(errors.empty());
}

TEST(ValidateSpecies, DetectsMissingNameKey)
{
  YAML::Node species_list = YAML::Load(R"(
    - "absolute tolerance": "1.0e-30"
      "is third body": true
    - "name": "B"
      "molecular weight [kg mol-1]": 0.034
  )");

  auto errors = development::ValidateSpecies(species_list);
  EXPECT_FALSE(errors.empty());
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

TEST(ValidateSpecies, DetectsInvalidKeysInSpecies)
{
  YAML::Node species_list = YAML::Load(R"(
    - "name": "A"
      "Absolute Tolerance": 1.0e-30
    - "name": "B"
      "absolute tolerance": 1.0e-30
  )");

  auto errors = development::ValidateSpecies(species_list);
  EXPECT_EQ(errors.size(), 1);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::InvalidKey };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

TEST(ValidateSpecies, DetectsDuplicateSpeciesNames)
{
  YAML::Node species_list = YAML::Load(R"(
    - "name": "A"
      "absolute tolerance": "1.0e-30"
    - "name": "B"
      "molecular weight [kg mol-1]": 0.034
    - "name": "A"
      "density [kg m-3]": 1000.0
  )");

  auto errors = development::ValidateSpecies(species_list);
  EXPECT_EQ(errors.size(), 2);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::DuplicateSpeciesDetected,
                                                ConfigParseStatus::DuplicateSpeciesDetected };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    EXPECT_NE(message.find("A"), std::string::npos);  // Error message should contain species "name"
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

TEST(ValidateSpecies, DetectsMultipleDuplicateSpecies)
{
  YAML::Node species_list = YAML::Load(R"(
    - "name": "A"
    - "name": "B"
    - "name": "A"
    - "name": "C"
    - "name": "B"
  )");

  auto errors = development::ValidateSpecies(species_list);
  EXPECT_EQ(errors.size(), 4);  // 2 for "A" duplicates + 2 for "B" duplicates

  for (const auto& [status, message] : errors)
  {
    EXPECT_EQ(status, ConfigParseStatus::DuplicateSpeciesDetected);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
}

TEST(ValidateSpecies, ValidatesAllOptionalKeys)
{
  YAML::Node species_list = YAML::Load(R"(
    - "name": "CompleteSpecies"
      "absolute tolerance": "1.0e-30"
      "diffusion coefficient [m2 s-1]": 1.46e-05
      "molecular weight [kg mol-1]": 0.0340147
      "HLC(298K) [mol m-3 Pa-1]": 1.011596348
      "HLC exponential factor [K]": 6340
      "N star": 1.74
      "density [kg m-3]": 1000.0
      "tracer type": "CHEM"
      "constant concentration [mol m-3]": 2.5e19
      "constant mixing ratio [mol mol-1]": 1.0e-6
      "is third body": true
  )");

  auto errors = development::ValidateSpecies(species_list);
  EXPECT_TRUE(errors.empty());
}
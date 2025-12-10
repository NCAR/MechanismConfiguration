#include <mechanism_configuration/development/mechanism.hpp>
#include <mechanism_configuration/development/type_validators.hpp>

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParsePhases, ParseValidConfig)
{
  development::Parser parser;

  std::string path = "development_unit_configs/phases/valid_phases";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 0) << "Validation errors were: " << validation_errors.size();

    auto mechanism = parser.Parse(object);
    EXPECT_EQ(mechanism.species.size(), 3);
    EXPECT_EQ(mechanism.phases.size(), 2);

    EXPECT_EQ(mechanism.phases[0].name, "gas");
    EXPECT_EQ(mechanism.phases[0].species.size(), 2);
    EXPECT_EQ(mechanism.phases[0].species[0].name, "A");
    EXPECT_TRUE(mechanism.phases[0].species[0].diffusion_coefficient.has_value());
    EXPECT_DOUBLE_EQ(mechanism.phases[0].species[0].diffusion_coefficient.value(), 4.23e-7);
    EXPECT_EQ(mechanism.phases[0].species[1].name, "B");
    EXPECT_FALSE(mechanism.phases[0].species[1].diffusion_coefficient.has_value());
    EXPECT_EQ(mechanism.phases[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.phases[0].unknown_properties["__other"], "This is a comment.");

    EXPECT_EQ(mechanism.phases[1].name, "aqueous");
    EXPECT_EQ(mechanism.phases[1].species.size(), 1);
    EXPECT_EQ(mechanism.phases[1].species[0].name, "C");
    EXPECT_TRUE(mechanism.phases[1].species[0].diffusion_coefficient.has_value());
    EXPECT_DOUBLE_EQ(mechanism.phases[1].species[0].diffusion_coefficient.value(), 4.23e-7);
    EXPECT_EQ(mechanism.phases[1].unknown_properties.size(), 2);
    EXPECT_EQ(mechanism.phases[1].unknown_properties["__other1"], "This is another comment.");
    EXPECT_EQ(mechanism.phases[1].unknown_properties["__other2"], "This is again a comment.");
  }
}

TEST(ParsePhases, DetectsDuplicatePhases)
{
  development::Parser parser;

  std::string path = "development_unit_configs/phases/duplicate_phases";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 2);

    for (const auto& [status, message] : validation_errors)
    {
      EXPECT_EQ(status, ConfigParseStatus::DuplicatePhasesDetected);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
  }
}

TEST(ParsePhases, DetectsMissingRequiredKeys)
{
  development::Parser parser;

  std::string path = "development_unit_configs/phases/missing_required_key";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParsePhases, DetectsInvalidKeys)
{
  development::Parser parser;

  std::string path = "development_unit_configs/phases/invalid_key";
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

TEST(ParsePhases, DetectsPhaseRequestingUnknownSpecies)
{
  development::Parser parser;

  std::string path = "development_unit_configs/phases/unknown_species";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::PhaseRequiresUnknownSpecies };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParsePhases, DetectsDuplicateSpeciesInPhase)
{
  development::Parser parser;

  std::string path = "development_unit_configs/phases/duplicate_species_in_phase";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 2);

    for (const auto& [status, message] : validation_errors)
    {
      EXPECT_EQ(status, ConfigParseStatus::DuplicateSpeciesInPhaseDetected);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
  }
}

TEST(ParsePhases, DetectsInvalidSpeciesObject)
{
  development::Parser parser;

  std::string path = "development_unit_configs/phases/invalid_species_object";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParsePhases, CanParsePhaseSpeciesProperties)
{
  development::Parser parser;

  std::string path = "development_unit_configs/phases/phase_species_properties";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 0) << "Validation errors were: " << validation_errors.size();

    auto mechanism = parser.Parse(object);
    EXPECT_EQ(mechanism.species.size(), 3);
    EXPECT_EQ(mechanism.phases.size(), 1);

    const auto& phase = mechanism.phases[0];
    EXPECT_EQ(phase.name, "my phase");
    EXPECT_EQ(phase.species.size(), 3);
    EXPECT_EQ(phase.unknown_properties.size(), 1);
    EXPECT_EQ(phase.unknown_properties.at("__my custom phase property"), "custom value");

    // Check first species with diffusion coefficient
    EXPECT_EQ(phase.species[0].name, "foo");
    EXPECT_TRUE(phase.species[0].diffusion_coefficient.has_value());
    EXPECT_EQ(phase.species[0].diffusion_coefficient.value(), 4.23e-7);
    EXPECT_EQ(phase.species[0].unknown_properties.size(), 0);

    // Check second species with custom properties
    EXPECT_EQ(phase.species[1].name, "bar");
    EXPECT_FALSE(phase.species[1].diffusion_coefficient.has_value());
    EXPECT_EQ(phase.species[1].unknown_properties.size(), 2);
    EXPECT_EQ(phase.species[1].unknown_properties.at("__custom property"), "0.5");
    EXPECT_EQ(phase.species[1].unknown_properties.at("__another custom property"), "value");

    // Check third species (simple string format)
    EXPECT_EQ(phase.species[2].name, "baz");
    EXPECT_FALSE(phase.species[2].diffusion_coefficient.has_value());
    EXPECT_EQ(phase.species[2].unknown_properties.size(), 0);
  }
}

TEST(ValidatePhases, ReturnsEmptyErrorsForValidPhases)
{
  std::vector<development::types::Species> existing_species;
  development::types::Species species1;
  species1.name = "A";
  existing_species.emplace_back(species1);

  development::types::Species species2;
  species2.name = "B";
  existing_species.emplace_back(species2);

  YAML::Node phases_list = YAML::Load(R"(
    - name: "gas"
      species:
        - name: "A"
        - name: "B"
          "diffusion coefficient [m2 s-1]": 1.5e-05
    - name: "aqueous"
      species:
        - name: "A"
          "diffusion coefficient [m2 s-1]": 2.3e-06
  )");

  auto errors = development::ValidatePhases(phases_list, existing_species);
  EXPECT_TRUE(errors.empty());
}

TEST(ValidatePhases, DetectsMissingPhaseName)
{
  std::vector<development::types::Species> existing_species;
  development::types::Species species1;
  species1.name = "A";
  existing_species.emplace_back(species1);

  YAML::Node phases_list = YAML::Load(R"(
    - species:
        - name: "A"
    - name: "aqueous"
      species:
        - name: "A"
  )");

  auto errors = development::ValidatePhases(phases_list, existing_species);
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

TEST(ValidatePhases, DetectsMissingSpeciesList)
{
  std::vector<development::types::Species> existing_species;
  development::types::Species species1;
  species1.name = "A";
  existing_species.emplace_back(species1);

  YAML::Node phases_list = YAML::Load(R"(
    - name: "gas"
    - name: "aqueous"
      species:
        - name: "A"
  )");

  auto errors = development::ValidatePhases(phases_list, existing_species);
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

TEST(ValidatePhases, DetectsInvalidKeysInPhase)
{
  std::vector<development::types::Species> existing_species;
  development::types::Species species1;
  species1.name = "A";
  existing_species.emplace_back(species1);

  YAML::Node phases_list = YAML::Load(R"(
    - name: "gas"
      SPECIES:
        - name: "A"
  )");

  auto errors = development::ValidatePhases(phases_list, existing_species);
  EXPECT_EQ(errors.size(), 2);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::InvalidKey, ConfigParseStatus::RequiredKeyNotFound };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

TEST(ValidatePhases, DetectsMissingSpeciesNameInPhase)
{
  std::vector<development::types::Species> existing_species;
  development::types::Species species1;
  species1.name = "A";
  existing_species.emplace_back(species1);

  YAML::Node phases_list = YAML::Load(R"(
    - name: "gas"
      species:
        - "diffusion coefficient [m2 s-1]": 1.5e-05
        - name: "A"
  )");

  auto errors = development::ValidatePhases(phases_list, existing_species);
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

TEST(ValidatePhases, DetectsInvalidKeysInSpecies)
{
  std::vector<development::types::Species> existing_species;
  development::types::Species species1;
  species1.name = "A";
  existing_species.emplace_back(species1);

  YAML::Node phases_list = YAML::Load(R"(
    - name: "gas"
      species:
        - name: "A"
          Coefficient: 4.23e-5
  )");

  auto errors = development::ValidatePhases(phases_list, existing_species);
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

TEST(ValidatePhases, DetectsDuplicateSpeciesInPhase)
{
  std::vector<development::types::Species> existing_species;
  development::types::Species species1;
  species1.name = "FOO";
  existing_species.emplace_back(species1);

  YAML::Node phases_list = YAML::Load(R"(
    - name: "gas"
      species:
        - name: "FOO"
        - name: "FOO"
          "diffusion coefficient [m2 s-1]": 1.5e-05
  )");

  auto errors = development::ValidatePhases(phases_list, existing_species);
  EXPECT_EQ(errors.size(), 2);  // Two entries for the duplicate species

  for (const auto& [status, message] : errors)
  {
    EXPECT_EQ(status, ConfigParseStatus::DuplicateSpeciesInPhaseDetected);
    EXPECT_NE(message.find("FOO"), std::string::npos);  // Error message should contain species name
  }
}

TEST(ValidatePhases, DetectsUnknownSpeciesInPhase)
{
  std::vector<development::types::Species> existing_species;
  development::types::Species species1;
  species1.name = "A";
  existing_species.emplace_back(species1);

  YAML::Node phases_list = YAML::Load(R"(
    - name: "gas"
      species:
        - name: "A"
        - name: "FOO"
  )");

  auto errors = development::ValidatePhases(phases_list, existing_species);
  EXPECT_EQ(errors.size(), 1);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::PhaseRequiresUnknownSpecies };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

TEST(ValidatePhases, DetectsDuplicatePhaseNames)
{
  std::vector<development::types::Species> existing_species;
  development::types::Species species1;
  species1.name = "A";
  existing_species.emplace_back(species1);

  development::types::Species species2;
  species2.name = "B";
  existing_species.emplace_back(species2);

  YAML::Node phases_list = YAML::Load(R"(
    - name: "gas"
      species:
        - name: "A"
    - name: "aqueous"
      species:
        - name: "B"
    - name: "gas"
      species:
        - name: "B"
  )");

  auto errors = development::ValidatePhases(phases_list, existing_species);
  EXPECT_EQ(errors.size(), 2);  // Two entries for the duplicate phase

  for (const auto& [status, message] : errors)
  {
    EXPECT_EQ(status, ConfigParseStatus::DuplicatePhasesDetected);
    EXPECT_NE(message.find("gas"), std::string::npos);  // Error message should contain phase name
  }
}

TEST(ValidatePhases, ValidatesAllSpeciesOptionalKeys)
{
  std::vector<development::types::Species> existing_species;
  development::types::Species species1;
  species1.name = "FOO";
  existing_species.emplace_back(species1);

  YAML::Node phases_list = YAML::Load(R"(
    - name: "organic"
      species:
        - name: "FOO"
          "diffusion coefficient [m2 s-1]": 1.46e-05
  )");

  auto errors = development::ValidatePhases(phases_list, existing_species);
  EXPECT_TRUE(errors.empty());
}
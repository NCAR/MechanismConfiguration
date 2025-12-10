#include <mechanism_configuration/development/mechanism.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParseGas, ParseValidConfig)
{
  development::Parser parser;

  std::string path = "development_unit_configs/models/gas/valid";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 0) << "Validation errors were: " << validation_errors.size();

    auto mechanism = parser.Parse(object);
    EXPECT_EQ(mechanism.models.gas_model.name, "gas");
    EXPECT_EQ(mechanism.models.gas_model.type, "GAS_PHASE");
    EXPECT_EQ(mechanism.models.gas_model.phase, "gas");
  }
}

TEST(ParseGas, MissingPhase)
{
  development::Parser parser;

  std::string path = "development_unit_configs/models/gas/missing_phase";
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

TEST(ParseGas, PhaseNotFoundInRegisteredPhases)
{
  development::Parser parser;

  std::string path = "development_unit_configs/models/gas/gas_phase_not_found_in_phases";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 2);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::UnknownPhase, ConfigParseStatus::UnknownPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : validation_errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}
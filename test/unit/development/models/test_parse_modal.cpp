#include <mechanism_configuration/development/mechanism.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParseModal, ParseValidConfig)
{
  development::Parser parser;

  std::string path = "development_unit_configs/models/modal/valid";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 0) << "Validation errors were: " << validation_errors.size();

    auto mechanism = parser.Parse(object);
    EXPECT_EQ(mechanism.models.modal_model.type, "MODAL");
    EXPECT_EQ(mechanism.models.modal_model.name, "aqueous");
    EXPECT_EQ(mechanism.models.modal_model.modes.size(), 2);
    EXPECT_EQ(mechanism.models.modal_model.modes[0].name, "aitken");
    EXPECT_EQ(mechanism.models.modal_model.modes[0].geometric_mean_diameter, 2.6e-8);
    EXPECT_EQ(mechanism.models.modal_model.modes[0].geometric_standard_deviation, 1.6);
    EXPECT_EQ(mechanism.models.modal_model.modes[0].phase, "aqueous");
    EXPECT_EQ(mechanism.models.modal_model.modes[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.models.modal_model.modes[0].unknown_properties["__comment"], "Aitken mode");

    EXPECT_EQ(mechanism.models.modal_model.modes[1].name, "accumulation");
    EXPECT_EQ(mechanism.models.modal_model.modes[1].geometric_mean_diameter, 1.1e-7);
    EXPECT_EQ(mechanism.models.modal_model.modes[1].geometric_standard_deviation, 1.8);
    EXPECT_EQ(mechanism.models.modal_model.modes[1].phase, "organic");
    EXPECT_EQ(mechanism.models.modal_model.modes[1].unknown_properties.size(), 0);
  }
}

TEST(ParseModal, MissingModes)
{
  development::Parser parser;

  std::string path = "development_unit_configs/models/modal/missing_modes";
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

TEST(ParseModal, MissingModalMemberData)
{
  development::Parser parser;

  std::string path = "development_unit_configs/models/modal/missing_modal_variable";
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    YAML::Node object = parser.FileToYaml(path + extension);

    auto validation_errors = parser.Validate(object);
    EXPECT_EQ(validation_errors.size(), 2);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound,
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

TEST(ParseModal, PhaseInModeNotFoundInRegisteredPhase)
{
  development::Parser parser;

  std::string path = "development_unit_configs/models/modal/mode_phase_not_found_in_phases";
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
#include <mechanism_configuration/development/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParseModal, ParseValidConfig)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/models/modal/valid") + extension);
    EXPECT_TRUE(parsed);

    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.models.modal_model.type, "MODAL");
    EXPECT_EQ(mechanism.models.modal_model.name, "aqueous");
    EXPECT_EQ(mechanism.models.modal_model.modes.size(), 2);
    EXPECT_EQ(mechanism.models.modal_model.modes[0].name, "aitken");
    EXPECT_EQ(mechanism.models.modal_model.modes[0].geometric_mean_diameter, 2.6e-8);
    EXPECT_EQ(mechanism.models.modal_model.modes[0].geometric_standard_deviation, 1.6);
    EXPECT_EQ(mechanism.models.modal_model.modes[0].phases.size(), 1);
    EXPECT_EQ(mechanism.models.modal_model.modes[0].phases[0], "aqueous");
    EXPECT_EQ(mechanism.models.modal_model.modes[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.models.modal_model.modes[0].unknown_properties["__comment"], "Aitken mode");

    EXPECT_EQ(mechanism.models.modal_model.modes[1].name, "accumulation");
    EXPECT_EQ(mechanism.models.modal_model.modes[1].geometric_mean_diameter, 1.1e-7);
    EXPECT_EQ(mechanism.models.modal_model.modes[1].geometric_standard_deviation, 1.8);
    EXPECT_EQ(mechanism.models.modal_model.modes[1].phases.size(), 2);
    EXPECT_EQ(mechanism.models.modal_model.modes[1].phases[0], "aqueous");
    EXPECT_EQ(mechanism.models.modal_model.modes[1].phases[1], "organic");
    EXPECT_EQ(mechanism.models.modal_model.modes[1].unknown_properties.size(), 0);
  }
}

TEST(ParseModal, MissingModes)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/models/modal/missing_modes") + extension);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
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
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/models/modal/missing_modal_variable") + extension);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound,
                                                  ConfigParseStatus::RequiredKeyNotFound };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
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
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed =
        parser.Parse(std::string("development_unit_configs/models/modal/mode_phase_not_found_in_phases") + extension);
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
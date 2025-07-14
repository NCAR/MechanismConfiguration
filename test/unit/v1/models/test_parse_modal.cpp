#include <mechanism_configuration/v1/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidModalModel)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/models/modal/valid") + extension);
    EXPECT_TRUE(parsed);

    v1::types::Mechanism mechanism = *parsed;

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

TEST(ParserBase, MissingModes)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/models/modal/missing_modes") + extension);
    EXPECT_FALSE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);

    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, MissingModalMemberData)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/models/modal/missing_modal_variable") + extension);
    EXPECT_FALSE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(parsed.errors.size(), 2);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, PhaseInModeNotFoundInRegisteredPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/models/modal/mode_phase_not_found_in_phases") + extension);
    EXPECT_FALSE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::UnknownPhase);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}
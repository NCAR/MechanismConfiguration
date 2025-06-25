#include <mechanism_configuration/v1/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidGasModel)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/models/gas/valid") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.models.gas_model.name, "gas");
    EXPECT_EQ(mechanism.models.gas_model.type, "GAS_PHASE");
    EXPECT_EQ(mechanism.models.gas_model.phase, "gas");
  }
}

TEST(ParserBase, GasModelMissingPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/models/gas/missing_phase") + extension);
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

TEST(ParserBase, GasModelPhaseNotFoundInRegisteredPhases)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/models/gas/gas_phase_not_found_in_phases") + extension);
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
#include <gtest/gtest.h>

#include <mechanism_configuration/v1/parser.hpp>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidPhases)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/phases/valid_phases") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;
    EXPECT_EQ(mechanism.species.size(), 3);
    EXPECT_EQ(mechanism.phases.size(), 2);

    EXPECT_EQ(mechanism.phases[0].name, "gas");
    EXPECT_EQ(mechanism.phases[0].species.size(), 2);
    EXPECT_EQ(mechanism.phases[0].species[0], "A");
    EXPECT_EQ(mechanism.phases[0].species[1], "B");
    EXPECT_EQ(mechanism.phases[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.phases[0].unknown_properties["__other"], "key");

    EXPECT_EQ(mechanism.phases[1].name, "aqueous");
    EXPECT_EQ(mechanism.phases[1].species.size(), 1);
    EXPECT_EQ(mechanism.phases[1].species[0], "C");
    EXPECT_EQ(mechanism.phases[1].unknown_properties.size(), 2);
    EXPECT_EQ(mechanism.phases[1].unknown_properties["__other1"], "key1");
    EXPECT_EQ(mechanism.phases[1].unknown_properties["__other2"], "key2");
  }
}

TEST(ParserBase, DetectsDuplicatePhases)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/phases/duplicate_phases") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::DuplicatePhasesDetected);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, DetectsMissingRequiredKeys)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/phases/missing_required_key") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, DetectsInvalidKeys)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/phases/invalid_key") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::InvalidKey);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, DetectsPhaseRequestingUnknownSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/phases/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::PhaseRequiresUnknownSpecies);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}
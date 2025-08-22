#include <mechanism_configuration/v1/parser.hpp>

#include <gtest/gtest.h>

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
    EXPECT_EQ(mechanism.phases[0].species[0].name, "A");
    EXPECT_EQ(mechanism.phases[0].species[1].name, "B");
    EXPECT_EQ(mechanism.phases[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.phases[0].unknown_properties["__other"], "key");

    EXPECT_EQ(mechanism.phases[1].name, "aqueous");
    EXPECT_EQ(mechanism.phases[1].species.size(), 1);
    EXPECT_EQ(mechanism.phases[1].species[0].name, "C");
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
    EXPECT_EQ(parsed.errors.size(), 2);
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

TEST(ParserBase, DetectsDuplicateSpeciesInPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/phases/duplicate_species_in_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::DuplicateSpeciesInPhaseDetected);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, CanParsePhaseSpeciesProperties)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/phases/phase_species_properties") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;
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
    EXPECT_EQ(phase.species[0].diffusion_coefficient.value(), 1.0);
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

TEST(ParserBase, DetectsInvalidSpeciesObject)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/phases/invalid_species_object") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_GE(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}
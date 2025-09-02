#include <mechanism_configuration/development/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidSpecies)
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

TEST(ParserBase, DetectsDuplicateSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/species/duplicate_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 4);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::DuplicateSpeciesDetected);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::DuplicateSpeciesDetected);
    EXPECT_EQ(parsed.errors[2].first, ConfigParseStatus::DuplicateSpeciesDetected);
    EXPECT_EQ(parsed.errors[3].first, ConfigParseStatus::DuplicateSpeciesDetected);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, DetectsMissingRequiredKeys)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/species/missing_required_key") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::InvalidKey);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, DetectsInvalidKeys)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/species/invalid_key") + extension;
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

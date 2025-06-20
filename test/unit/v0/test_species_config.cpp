#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v0/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(SpeciesConfig, ValidSpeciesConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/species/valid/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_TRUE(parsed);
    if (!parsed)
    {
      for (auto& error : parsed.errors)
      {
        std::cerr << error.second << std::endl;
      }
    }
    v0::types::Mechanism mechanism = *parsed;

    auto& species_vector = mechanism.species;
    EXPECT_EQ(species_vector.size(), 4);

    // first species
    {
      EXPECT_EQ(species_vector[0].name, "foo");
      EXPECT_EQ(species_vector[0].molecular_weight, 0.123);
      EXPECT_EQ(species_vector[0].diffusion_coefficient, 2.3e-4);
      EXPECT_FALSE(species_vector[0].absolute_tolerance.has_value());
    }

    // second species
    {
      EXPECT_EQ(species_vector[1].name, "bar");
      EXPECT_EQ(species_vector[1].molecular_weight, 0.321);
      EXPECT_EQ(species_vector[1].diffusion_coefficient, 0.4e-5);
      EXPECT_FALSE(species_vector[1].absolute_tolerance.has_value());
    }

    // third species
    {
      EXPECT_EQ(species_vector[2].name, "baz");
      EXPECT_FALSE(species_vector[2].molecular_weight.has_value());
      EXPECT_FALSE(species_vector[2].diffusion_coefficient.has_value());
      EXPECT_EQ(species_vector[2].absolute_tolerance, 1e-10);
    }

    // fourth species
    {
      EXPECT_EQ(species_vector[3].name, "quz");
      EXPECT_FALSE(species_vector[3].molecular_weight.has_value());
      EXPECT_FALSE(species_vector[3].diffusion_coefficient.has_value());
      EXPECT_FALSE(species_vector[3].absolute_tolerance.has_value());
    }
  }
}

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v0/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(SurfaceConfig, DetectsInvalidConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/surface/missing_reactants/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/surface/missing_products/config" + extension;
    parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/surface/missing_MUSICA_name/config" + extension;
    parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(SurfaceConfig, ParseConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/surface/valid/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_TRUE(parsed);
    v0::types::Mechanism mechanism = *parsed;

    auto& process_vector = mechanism.reactions.surface;
    EXPECT_EQ(process_vector.size(), 2);

    // first reaction
    {
      EXPECT_EQ(process_vector[0].gas_phase_species.species_name, "foo");
      EXPECT_EQ(process_vector[0].gas_phase_products.size(), 2);
      EXPECT_EQ(process_vector[0].gas_phase_products[0].species_name, "bar");
      EXPECT_EQ(process_vector[0].gas_phase_products[0].coefficient, 1.0);
      EXPECT_EQ(process_vector[0].gas_phase_products[1].species_name, "baz");
      EXPECT_EQ(process_vector[0].gas_phase_products[1].coefficient, 3.2);
      EXPECT_EQ(process_vector[0].name, "SURF.kfoo");
      EXPECT_EQ(process_vector[0].reaction_probability, 1.0);
      auto it =
          std::find_if(mechanism.species.begin(), mechanism.species.end(), [](const v0::types::Species& species) { return species.name == "foo"; });
      ASSERT_NE(it, mechanism.species.end());
      EXPECT_EQ(it->diffusion_coefficient, 2.3e-4);
    }

    // second reaction
    {
      EXPECT_EQ(process_vector[1].gas_phase_species.species_name, "bar");
      EXPECT_EQ(process_vector[1].gas_phase_products.size(), 2);
      EXPECT_EQ(process_vector[1].gas_phase_products[0].species_name, "bar");
      EXPECT_EQ(process_vector[1].gas_phase_products[0].coefficient, 0.5);
      EXPECT_EQ(process_vector[1].gas_phase_products[1].species_name, "foo");
      EXPECT_EQ(process_vector[1].gas_phase_products[1].coefficient, 1.0);
      EXPECT_EQ(process_vector[1].name, "SURF.kbar");
      EXPECT_EQ(process_vector[1].reaction_probability, 0.5);
      auto it =
          std::find_if(mechanism.species.begin(), mechanism.species.end(), [](const v0::types::Species& species) { return species.name == "bar"; });
      ASSERT_NE(it, mechanism.species.end());
      EXPECT_EQ(it->diffusion_coefficient, 0.4e-5);
    }
  }
}

TEST(SurfaceConfig, DetectsNonstandardKeys)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/surface/contains_nonstandard_key/config" + extension;
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

TEST(SurfaceConfig, DetectsNonstandardProductCoefficient)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/surface/nonstandard_product_coef/config" + extension;
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
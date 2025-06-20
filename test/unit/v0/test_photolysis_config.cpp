#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v0/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(PhotolysisConfig, DetectsInvalidConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/photolysis/missing_reactants/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/photolysis/missing_products/config" + extension;
    parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/photolysis/missing_MUSICA_name/config" + extension;
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

TEST(PhotolysisConfig, ParseConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/photolysis/valid/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_TRUE(parsed);
    v0::types::Mechanism mechanism = *parsed;

    auto& process_vector = mechanism.reactions.user_defined;
    EXPECT_EQ(process_vector.size(), 2);

    // first reaction
    {
      EXPECT_EQ(process_vector[0].reactants.size(), 1);
      EXPECT_EQ(process_vector[0].reactants[0].species_name, "foo");
      EXPECT_EQ(process_vector[0].products.size(), 2);
      EXPECT_EQ(process_vector[0].products[0].species_name, "bar");
      EXPECT_EQ(process_vector[0].products[0].coefficient, 1.0);
      EXPECT_EQ(process_vector[0].products[1].species_name, "baz");
      EXPECT_EQ(process_vector[0].products[1].coefficient, 3.2);
      EXPECT_EQ(process_vector[0].name, "PHOTO.jfoo");
      EXPECT_EQ(process_vector[0].scaling_factor, 1.0);
    }

    // second reaction
    {
      EXPECT_EQ(process_vector[1].reactants.size(), 1);
      EXPECT_EQ(process_vector[1].reactants[0].species_name, "bar");
      EXPECT_EQ(process_vector[1].products.size(), 2);
      EXPECT_EQ(process_vector[1].products[0].species_name, "bar");
      EXPECT_EQ(process_vector[1].products[0].coefficient, 0.5);
      EXPECT_EQ(process_vector[1].products[1].species_name, "foo");
      EXPECT_EQ(process_vector[1].products[1].coefficient, 1.0);
      EXPECT_EQ(process_vector[1].name, "PHOTO.jbar");
      EXPECT_EQ(process_vector[1].scaling_factor, 2.5);
    }
  }
}

TEST(PhotolysisConfig, DetectsNonstandardKeys)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/photolysis/contains_nonstandard_key/config" + extension;
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

TEST(PhotolysisConfig, DetectsNonstandardProductCoefficient)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/photolysis/nonstandard_product_coef/config" + extension;
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

TEST(PhotolysisConfig, DetectsNonstandardReactantCoefficient)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/photolysis/nonstandard_reactant_coef/config" + extension;
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
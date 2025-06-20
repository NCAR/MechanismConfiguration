#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/conversions.hpp>
#include <mechanism_configuration/v0/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(BranchedConfig, DetectsInvalidConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/branched/missing_reactants/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 5);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[2].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[3].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[4].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/branched/missing_alkoxy_products/config" + extension;
    parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 5);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[2].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[3].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[4].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/branched/missing_nitrate_products/config" + extension;
    parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 5);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[2].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[3].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[4].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(BranchedConfig, ParseConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/branched/valid/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_TRUE(parsed);
    v0::types::Mechanism mechanism = *parsed;

    auto& process_vector = mechanism.reactions.branched;
    EXPECT_EQ(process_vector.size(), 2);

    // first reaction
    EXPECT_EQ(process_vector[0].reactants.size(), 2);
    EXPECT_EQ(process_vector[0].reactants[0].species_name, "foo");
    EXPECT_EQ(process_vector[0].reactants[0].coefficient, 1.0);
    EXPECT_EQ(process_vector[0].reactants[1].species_name, "quz");
    EXPECT_EQ(process_vector[0].reactants[1].coefficient, 2.0);
    EXPECT_EQ(process_vector[0].X, 12.3 * std::pow(conversions::MolesM3ToMoleculesCm3, 2));
    EXPECT_EQ(process_vector[0].Y, 42.3);
    EXPECT_EQ(process_vector[0].a0, 1.0e-5);
    EXPECT_EQ(process_vector[0].n, 3);
    EXPECT_EQ(process_vector[0].alkoxy_products.size(), 2);
    EXPECT_EQ(process_vector[0].alkoxy_products[0].species_name, "bar");
    EXPECT_EQ(process_vector[0].alkoxy_products[0].coefficient, 1.0);
    EXPECT_EQ(process_vector[0].alkoxy_products[1].species_name, "baz");
    EXPECT_EQ(process_vector[0].alkoxy_products[1].coefficient, 3.2);
    EXPECT_EQ(process_vector[0].nitrate_products.size(), 1);
    EXPECT_EQ(process_vector[0].nitrate_products[0].species_name, "quz");
    EXPECT_EQ(process_vector[0].nitrate_products[0].coefficient, 1.0);

    // second reaction
    EXPECT_EQ(process_vector[1].X, 0.32 * conversions::MolesM3ToMoleculesCm3);
    EXPECT_EQ(process_vector[1].Y, 2.3e8);
    EXPECT_EQ(process_vector[1].a0, 0.423);
    EXPECT_EQ(process_vector[1].alkoxy_products.size(), 1);
    EXPECT_EQ(process_vector[1].alkoxy_products[0].coefficient, 1.0);
    EXPECT_EQ(process_vector[1].alkoxy_products[0].species_name, "baz");
    EXPECT_EQ(process_vector[1].n, 6);
    EXPECT_EQ(process_vector[1].nitrate_products.size(), 2);
    EXPECT_EQ(process_vector[1].nitrate_products[0].coefficient, 0.5);
    EXPECT_EQ(process_vector[1].nitrate_products[0].species_name, "bar");
    EXPECT_EQ(process_vector[1].nitrate_products[1].coefficient, 1.0);
    EXPECT_EQ(process_vector[1].nitrate_products[1].species_name, "foo");
    EXPECT_EQ(process_vector[1].reactants.size(), 2);
    EXPECT_EQ(process_vector[1].reactants[0].species_name, "bar");
    EXPECT_EQ(process_vector[1].reactants[0].coefficient, 1.0);
    EXPECT_EQ(process_vector[1].reactants[1].species_name, "baz");
    EXPECT_EQ(process_vector[1].reactants[1].coefficient, 1.0);
  }
}

TEST(BranchedConfig, DetectsNonstandardKeys)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/branched/contains_nonstandard_key/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::InvalidKey);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::InvalidKey);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(BranchedConfig, DetectsNonstandardProductCoefficient)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/branched/nonstandard_alkoxy_product_coef/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::InvalidKey);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/branched/nonstandard_nitrate_product_coef/config" + extension;
    parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::InvalidKey);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(BranchedConfig, DetectsNonstandardReactantCoefficient)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/branched/nonstandard_reactant_coef/config" + extension;
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
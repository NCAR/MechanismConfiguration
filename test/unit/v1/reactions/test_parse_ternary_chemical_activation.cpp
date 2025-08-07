#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/conversions.hpp>
#include <mechanism_configuration/v1/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(TernaryChemicalActivationConfig, ParseValidConfig)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string file = "./v1_unit_configs/reactions/ternary_chemical_activation/valid/config" + extension;
    auto parsed = parser.Parse(file);
    if (!parsed) {
      for (auto& error : parsed.errors)
      {
        std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
      }
    }
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    auto& process_vector = mechanism.reactions.ternary_chemical_activation;
    EXPECT_EQ(process_vector.size(), 2);

    // first reaction
    {
      EXPECT_EQ(process_vector[0].reactants.size(), 2);
      EXPECT_EQ(process_vector[0].reactants[0].species_name, "foo");
      EXPECT_EQ(process_vector[0].reactants[0].coefficient, 1.0);
      EXPECT_EQ(process_vector[0].reactants[1].species_name, "quz");
      EXPECT_EQ(process_vector[0].reactants[1].coefficient, 2.0);
      EXPECT_EQ(process_vector[0].products.size(), 2);
      EXPECT_EQ(process_vector[0].products[0].species_name, "bar");
      EXPECT_EQ(process_vector[0].products[0].coefficient, 1.0);
      EXPECT_EQ(process_vector[0].products[1].species_name, "baz");
      EXPECT_EQ(process_vector[0].products[1].coefficient, 3.2);
      EXPECT_EQ(process_vector[0].k0_A, 1.0 * std::pow(conversions::MolesM3ToMoleculesCm3, 3));
      EXPECT_EQ(process_vector[0].k0_B, 0.0);
      EXPECT_EQ(process_vector[0].k0_C, 0.0);
      EXPECT_EQ(process_vector[0].kinf_A, 1.0 * std::pow(conversions::MolesM3ToMoleculesCm3, 2));
      EXPECT_EQ(process_vector[0].kinf_B, 0.0);
      EXPECT_EQ(process_vector[0].kinf_C, 0.0);
      EXPECT_EQ(process_vector[0].Fc, 0.6);
      EXPECT_EQ(process_vector[0].N, 1.0);
    }

    // second reaction
    {
      EXPECT_EQ(process_vector[1].reactants.size(), 2);
      EXPECT_EQ(process_vector[1].reactants[0].species_name, "bar");
      EXPECT_EQ(process_vector[1].reactants[1].species_name, "baz");
      EXPECT_EQ(process_vector[1].products.size(), 2);
      EXPECT_EQ(process_vector[1].products[0].species_name, "bar");
      EXPECT_EQ(process_vector[1].products[0].coefficient, 0.5);
      EXPECT_EQ(process_vector[1].products[1].species_name, "foo");
      EXPECT_EQ(process_vector[1].products[1].coefficient, 0.0);
      EXPECT_EQ(process_vector[1].k0_A, 32.1 * std::pow(conversions::MolesM3ToMoleculesCm3, 2));
      EXPECT_EQ(process_vector[1].k0_B, -2.3);
      EXPECT_EQ(process_vector[1].k0_C, 102.3);
      EXPECT_EQ(process_vector[1].kinf_A, 63.4 * std::pow(conversions::MolesM3ToMoleculesCm3, 1));
      EXPECT_EQ(process_vector[1].kinf_B, -1.3);
      EXPECT_EQ(process_vector[1].kinf_C, 908.5);
      EXPECT_EQ(process_vector[1].Fc, 1.3);
      EXPECT_EQ(process_vector[1].N, 32.1);
    }
  }
}

TEST(TernaryChemicalActivationConfig, DetectsNonStandardKey)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v1_unit_configs/reactions/ternary_chemical_activation/contains_nonstandard_key/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 12);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::InvalidKey);
    EXPECT_EQ(parsed.errors[2].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[3].first, ConfigParseStatus::InvalidKey);
    EXPECT_EQ(parsed.errors[4].first, ConfigParseStatus::InvalidKey);
    EXPECT_EQ(parsed.errors[5].first, ConfigParseStatus::InvalidKey);
    EXPECT_EQ(parsed.errors[6].first, ConfigParseStatus::InvalidKey);
    EXPECT_EQ(parsed.errors[7].first, ConfigParseStatus::InvalidKey);
    EXPECT_EQ(parsed.errors[8].first, ConfigParseStatus::InvalidKey);
    EXPECT_EQ(parsed.errors[9].first, ConfigParseStatus::InvalidKey);
    EXPECT_EQ(parsed.errors[10].first, ConfigParseStatus::InvalidKey);
    EXPECT_EQ(parsed.errors[11].first, ConfigParseStatus::InvalidKey);

    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}


TEST(TernaryChemicalActivationConfig, DetectsMissingProducts)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v1_unit_configs/reactions/ternary_chemical_activation/missing_products/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(TernaryChemicalActivationConfig, DetectsMissingReactants)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v1_unit_configs/reactions/ternary_chemical_activation/missing_reactants/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}
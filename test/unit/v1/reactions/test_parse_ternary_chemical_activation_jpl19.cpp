// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/constants.hpp"
#include "detail/conversions.hpp"
#include "utils/print.hpp"

#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(TernaryChemicalActivationJPL19Config, ParseValidConfig)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string file = "./v1_unit_configs/reactions/ternary_chemical_activation_jpl19/valid/config" + extension;
    auto parsed = Parse(file);
    if (!parsed)
    {
      for (auto& error : parsed.error())
      {
        std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
      }
    }
    ASSERT_TRUE(parsed);
    Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.ternary_chemical_activation.size(), 0);
    auto& process_vector = mechanism.reactions.ternary_chemical_activation_jpl19;
    ASSERT_EQ(process_vector.size(), 2);

    // first reaction uses the defaults
    {
      EXPECT_EQ(process_vector[0].gas_phase, "gas");
      EXPECT_EQ(process_vector[0].name, "");
      EXPECT_EQ(process_vector[0].reactants.size(), 2);
      EXPECT_EQ(process_vector[0].reactants[0].name, "foo");
      EXPECT_EQ(process_vector[0].reactants[0].coefficient, 1.0);
      EXPECT_EQ(process_vector[0].reactants[1].name, "quz");
      EXPECT_EQ(process_vector[0].reactants[1].coefficient, 2.0);
      EXPECT_EQ(process_vector[0].products.size(), 2);
      EXPECT_EQ(process_vector[0].products[0].name, "bar");
      EXPECT_EQ(process_vector[0].products[0].coefficient, 1.0);
      EXPECT_EQ(process_vector[0].products[1].name, "baz");
      EXPECT_EQ(process_vector[0].products[1].coefficient, 3.2);
      EXPECT_EQ(process_vector[0].k0_A, 1.0);
      EXPECT_EQ(process_vector[0].k0_B, 0.0);
      EXPECT_EQ(process_vector[0].k0_C, 0.0);
      EXPECT_EQ(process_vector[0].k0_D, 298.0);
      EXPECT_EQ(process_vector[0].kinf_A, 1.0);
      EXPECT_EQ(process_vector[0].kinf_B, 0.0);
      EXPECT_EQ(process_vector[0].kinf_C, 0.0);
      EXPECT_EQ(process_vector[0].kinf_D, 298.0);
      EXPECT_EQ(process_vector[0].kint_A, 1.0);
      EXPECT_EQ(process_vector[0].kint_B, 0.0);
      EXPECT_EQ(process_vector[0].kint_C, 0.0);
      EXPECT_EQ(process_vector[0].kint_D, 298.0);
      EXPECT_EQ(process_vector[0].Fc, 0.6);
      EXPECT_EQ(process_vector[0].N, 1.0);
    }

    // second reaction sets every parameter
    {
      EXPECT_EQ(process_vector[1].unknown_properties.size(), 1);
      EXPECT_EQ(process_vector[1].unknown_properties["__optional thing"], "hello");
      EXPECT_EQ(process_vector[1].reactants.size(), 2);
      EXPECT_EQ(process_vector[1].reactants[0].name, "bar");
      EXPECT_EQ(process_vector[1].reactants[1].name, "baz");
      EXPECT_EQ(process_vector[1].products.size(), 2);
      EXPECT_EQ(process_vector[1].products[0].name, "bar");
      EXPECT_EQ(process_vector[1].products[0].coefficient, 0.5);
      EXPECT_EQ(process_vector[1].products[1].name, "foo");
      EXPECT_EQ(process_vector[1].products[1].coefficient, 0.0);
      EXPECT_EQ(process_vector[1].k0_A, 32.1);
      EXPECT_EQ(process_vector[1].k0_B, -2.3);
      EXPECT_EQ(process_vector[1].k0_C, 102.3);
      EXPECT_EQ(process_vector[1].k0_D, 300.0);
      EXPECT_EQ(process_vector[1].kinf_A, 63.4);
      EXPECT_EQ(process_vector[1].kinf_B, -1.3);
      EXPECT_EQ(process_vector[1].kinf_C, 908.5);
      EXPECT_EQ(process_vector[1].kinf_D, 290.0);
      EXPECT_EQ(process_vector[1].kint_A, 1.2e-11);
      EXPECT_EQ(process_vector[1].kint_B, 0.5);
      EXPECT_EQ(process_vector[1].kint_C, -250.0);
      EXPECT_EQ(process_vector[1].kint_D, 310.0);
      EXPECT_EQ(process_vector[1].Fc, 1.3);
      EXPECT_EQ(process_vector[1].N, 32.1);
      EXPECT_EQ(process_vector[1].name, "my ternary chemical activation jpl19");
    }
  }
}

TEST(TernaryChemicalActivationJPL19Config, DetectsNonStandardKey)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        "./v1_unit_configs/reactions/ternary_chemical_activation_jpl19/contains_nonstandard_key/config" + extension;
    auto parsed = Parse(file);
    ASSERT_FALSE(parsed);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
    // 'Reactants' and 'Products' each give a missing key and an invalid key; the third
    // reaction has all 14 parameters in lower case.
    ASSERT_EQ(parsed.error().size(), 18);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::RequiredKeyNotFound);
    EXPECT_EQ(parsed.error()[1].first, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.error()[2].first, ErrorCode::RequiredKeyNotFound);
    for (std::size_t i = 3; i < parsed.error().size(); ++i)
    {
      EXPECT_EQ(parsed.error()[i].first, ErrorCode::InvalidKey) << i;
    }
  }
}

TEST(TernaryChemicalActivationJPL19Config, DetectsMissingProducts)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v1_unit_configs/reactions/ternary_chemical_activation_jpl19/missing_products/config" + extension;
    auto parsed = Parse(file);
    ASSERT_FALSE(parsed);
    ASSERT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::RequiredKeyNotFound);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(TernaryChemicalActivationJPL19Config, DetectsMissingReactants)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v1_unit_configs/reactions/ternary_chemical_activation_jpl19/missing_reactants/config" + extension;
    auto parsed = Parse(file);
    ASSERT_FALSE(parsed);
    ASSERT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::RequiredKeyNotFound);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(TernaryChemicalActivationJPL19Config, DetectsUnknownSpecies)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v1_unit_configs/reactions/ternary_chemical_activation_jpl19/unknown_species/config" + extension;
    auto parsed = Parse(file);
    ASSERT_FALSE(parsed);
    ASSERT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::ReactionRequiresUnknownSpecies);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

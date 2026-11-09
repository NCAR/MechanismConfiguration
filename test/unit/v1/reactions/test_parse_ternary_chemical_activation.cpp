// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/constants.hpp"
#include "detail/conversions.hpp"
#include "utils/print.hpp"

#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(TernaryChemicalActivationConfig, ParseValidConfig)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string file = "./v1_unit_configs/reactions/ternary_chemical_activation/valid/config" + extension;
    auto parsed = Parse(file);
    if (!parsed)
    {
      for (auto& error : parsed.errors)
      {
        std::cout << error.message << " " << ErrorCodeToString(error.code) << std::endl;
      }
    }
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed.mechanism;

    auto& process_vector = mechanism.reactions.ternary_chemical_activation;
    EXPECT_EQ(process_vector.size(), 2);

    // first reaction
    {
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
      EXPECT_EQ(process_vector[0].kinf_A, 1.0);
      EXPECT_EQ(process_vector[0].kinf_B, 0.0);
      EXPECT_EQ(process_vector[0].kinf_C, 0.0);
      EXPECT_EQ(process_vector[0].Fc, 0.6);
      EXPECT_EQ(process_vector[0].N, 1.0);
    }

    // second reaction
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
      EXPECT_EQ(process_vector[1].kinf_A, 63.4);
      EXPECT_EQ(process_vector[1].kinf_B, -1.3);
      EXPECT_EQ(process_vector[1].kinf_C, 908.5);
      EXPECT_EQ(process_vector[1].Fc, 1.3);
      EXPECT_EQ(process_vector[1].N, 32.1);
      EXPECT_EQ(process_vector[1].name, "my ternary chemical activation");
    }
  }
}

TEST(TernaryChemicalActivationConfig, DetectsNonStandardKey)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v1_unit_configs/reactions/ternary_chemical_activation/contains_nonstandard_key/config" + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    // The first two reactions are left out, so the last two errors tell that.
    EXPECT_EQ(parsed.errors.size(), 14);
    EXPECT_EQ(parsed.errors[0].code, ErrorCode::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[1].code, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.errors[2].code, ErrorCode::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[3].code, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.errors[4].code, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.errors[5].code, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.errors[6].code, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.errors[7].code, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.errors[8].code, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.errors[9].code, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.errors[10].code, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.errors[11].code, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.errors[12].code, ErrorCode::ItemLeftOut);
    EXPECT_EQ(parsed.errors[13].code, ErrorCode::ItemLeftOut);

    for (auto& error : parsed.errors)
    {
      std::cout << error.message << " " << ErrorCodeToString(error.code) << std::endl;
    }
  }
}

TEST(TernaryChemicalActivationConfig, DetectsMissingProducts)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v1_unit_configs/reactions/ternary_chemical_activation/missing_products/config" + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    for (auto& error : parsed.errors)
    {
      std::cout << error.message << " " << ErrorCodeToString(error.code) << std::endl;
    }
  }
}

TEST(TernaryChemicalActivationConfig, DetectsMissingReactants)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v1_unit_configs/reactions/ternary_chemical_activation/missing_reactants/config" + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    for (auto& error : parsed.errors)
    {
      std::cout << error.message << " " << ErrorCodeToString(error.code) << std::endl;
    }
  }
}
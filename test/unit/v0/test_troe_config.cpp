// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/constants.hpp"
#include "detail/conversions.hpp"
#include "detail/v0/parser.hpp"

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(TroeConfig, DetectsInvalidConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/troe/missing_reactants/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::RequiredKeyNotFound);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/troe/missing_products/config" + extension;
    parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::RequiredKeyNotFound);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(TroeConfig, ParseConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/troe/valid/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed;

    auto& process_vector = mechanism.reactions.troe;
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
      EXPECT_EQ(process_vector[1].reactants[0].name, "bar");
      EXPECT_EQ(process_vector[1].reactants[1].name, "baz");
      EXPECT_EQ(process_vector[1].products.size(), 2);
      EXPECT_EQ(process_vector[1].products[0].name, "bar");
      EXPECT_EQ(process_vector[1].products[0].coefficient, 0.5);
      EXPECT_EQ(process_vector[1].products[1].name, "foo");
      EXPECT_EQ(process_vector[1].products[1].coefficient, 1.0);
      EXPECT_EQ(process_vector[1].k0_A, 32.1 * std::pow(conversions::MolesM3ToMoleculesCm3, 2));
      EXPECT_EQ(process_vector[1].k0_B, -2.3);
      EXPECT_EQ(process_vector[1].k0_C, 102.3);
      EXPECT_EQ(process_vector[1].kinf_A, 63.4 * conversions::MolesM3ToMoleculesCm3);
      EXPECT_EQ(process_vector[1].kinf_B, -1.3);
      EXPECT_EQ(process_vector[1].kinf_C, 908.5);
      EXPECT_EQ(process_vector[1].Fc, 1.3);
      EXPECT_EQ(process_vector[1].N, 32.1);
    }
  }
}

TEST(TroeConfig, DetectsNonstandardKeys)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/troe/contains_nonstandard_key/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 8);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.error()[1].first, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.error()[2].first, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.error()[3].first, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.error()[4].first, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.error()[5].first, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.error()[6].first, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.error()[7].first, ErrorCode::InvalidKey);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(TroeConfig, DetectsNonstandardProductCoefficient)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/troe/nonstandard_product_coef/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::InvalidKey);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(TroeConfig, DetectsNonstandardReactantCoefficient)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/troe/nonstandard_reactant_coef/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::InvalidKey);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}
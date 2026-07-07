// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/constants.hpp"
#include "detail/conversions.hpp"
#include "detail/v0/parser.hpp"
#include "utils/print.hpp"

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ArrheniusConfig, DetectsInvalidConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/arrhenius/missing_reactants/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::RequiredKeyNotFound);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/arrhenius/missing_products/config" + extension;
    parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::RequiredKeyNotFound);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/arrhenius/mutually_exclusive/config" + extension;
    parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::MutuallyExclusiveOption);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ArrheniusConfig, ParseConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/arrhenius/valid/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed;

    // first reaction
    {
      EXPECT_EQ(mechanism.reactions.arrhenius[0].reactants.size(), 2);
      EXPECT_EQ(mechanism.reactions.arrhenius[0].reactants[0].name, "foo");
      EXPECT_EQ(mechanism.reactions.arrhenius[0].reactants[1].name, "quz");
      EXPECT_EQ(mechanism.reactions.arrhenius[0].products.size(), 2);
      EXPECT_EQ(mechanism.reactions.arrhenius[0].products[0].name, "bar");
      EXPECT_EQ(mechanism.reactions.arrhenius[0].products[0].coefficient, 1.0);
      EXPECT_EQ(mechanism.reactions.arrhenius[0].products[1].name, "baz");
      EXPECT_EQ(mechanism.reactions.arrhenius[0].products[1].coefficient, 3.2);
      EXPECT_EQ(
          mechanism.reactions.arrhenius[0].A, 1.0 * conversions::MolesM3ToMoleculesCm3 * conversions::MolesM3ToMoleculesCm3);
      EXPECT_EQ(mechanism.reactions.arrhenius[0].B, 0.0);
      EXPECT_EQ(mechanism.reactions.arrhenius[0].C, 0.0);
      EXPECT_EQ(mechanism.reactions.arrhenius[0].D, 300);
      EXPECT_EQ(mechanism.reactions.arrhenius[0].E, 0.0);
    }

    // second reaction
    {
      EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants.size(), 2);
      EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants[0].name, "bar");
      EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants[1].name, "baz");
      EXPECT_EQ(mechanism.reactions.arrhenius[1].products.size(), 2);
      EXPECT_EQ(mechanism.reactions.arrhenius[1].products[0].name, "bar");
      EXPECT_EQ(mechanism.reactions.arrhenius[1].products[0].coefficient, 0.5);
      EXPECT_EQ(mechanism.reactions.arrhenius[1].products[1].name, "foo");
      EXPECT_EQ(mechanism.reactions.arrhenius[1].products[1].coefficient, 1.0);
      EXPECT_EQ(mechanism.reactions.arrhenius[1].A, 32.1 * conversions::MolesM3ToMoleculesCm3);
      EXPECT_EQ(mechanism.reactions.arrhenius[1].B, -2.3);
      EXPECT_EQ(mechanism.reactions.arrhenius[1].C, 102.3);
      EXPECT_EQ(mechanism.reactions.arrhenius[1].D, 63.4);
      EXPECT_EQ(mechanism.reactions.arrhenius[1].E, -1.3);
    }

    // third reaction
    {
      EXPECT_EQ(mechanism.reactions.arrhenius[2].reactants.size(), 2);
      EXPECT_EQ(mechanism.reactions.arrhenius[2].reactants[0].name, "bar");
      EXPECT_EQ(mechanism.reactions.arrhenius[2].reactants[1].name, "baz");
      EXPECT_EQ(mechanism.reactions.arrhenius[2].products.size(), 2);
      EXPECT_EQ(mechanism.reactions.arrhenius[2].products[0].name, "bar");
      EXPECT_EQ(mechanism.reactions.arrhenius[2].products[0].coefficient, 0.5);
      EXPECT_EQ(mechanism.reactions.arrhenius[2].products[1].name, "foo");
      EXPECT_EQ(mechanism.reactions.arrhenius[2].products[1].coefficient, 1.0);
      EXPECT_EQ(mechanism.reactions.arrhenius[2].A, 32.1 * conversions::MolesM3ToMoleculesCm3);
      EXPECT_EQ(mechanism.reactions.arrhenius[2].B, -2.3);
      EXPECT_EQ(mechanism.reactions.arrhenius[2].C, -1 * 2e23 / constants::boltzmann);
      EXPECT_EQ(mechanism.reactions.arrhenius[2].D, 63.4);
      EXPECT_EQ(mechanism.reactions.arrhenius[2].E, -1.3);
    }
  }
}

TEST(ArrheniusConfig, DetectsNonstandardKeys)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/arrhenius/contains_nonstandard_key/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 3);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.error()[1].first, ErrorCode::InvalidKey);
    EXPECT_EQ(parsed.error()[2].first, ErrorCode::InvalidKey);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ArrheniusConfig, DetectsNonstandardProductCoefficient)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/arrhenius/nonstandard_product_coef/config" + extension;
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

TEST(ArrheniusConfig, DetectsNonstandardReactantCoefficient)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/arrhenius/nonstandard_reactant_coef/config" + extension;
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

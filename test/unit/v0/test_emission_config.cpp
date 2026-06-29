// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/constants.hpp"
#include "detail/v0/parser.hpp"

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(EmissionConfig, DetectsInvalidConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/emission/missing_products/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::RequiredKeyNotFound);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/emission/missing_MUSICA_name/config" + extension;
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

TEST(EmissionConfig, ParseConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/emission/valid/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed;

    auto& process_vector = mechanism.reactions.emission;
    EXPECT_EQ(process_vector.size(), 2);

    // first reaction
    {
      EXPECT_EQ(process_vector[0].reactants.size(), 0);
      EXPECT_EQ(process_vector[0].products.size(), 1);
      EXPECT_EQ(process_vector[0].products[0].name, "foo");
      EXPECT_EQ(process_vector[0].products[0].coefficient, 1.0);
      EXPECT_EQ(process_vector[0].name, "EMIS.foo");
      EXPECT_EQ(process_vector[0].scaling_factor, 1.0);
    }

    // second reaction
    {
      EXPECT_EQ(process_vector[1].reactants.size(), 0);
      EXPECT_EQ(process_vector[1].products.size(), 1);
      EXPECT_EQ(process_vector[1].products[0].name, "bar");
      EXPECT_EQ(process_vector[1].products[0].coefficient, 1.0);
      EXPECT_EQ(process_vector[1].name, "EMIS.bar");
      EXPECT_EQ(process_vector[1].scaling_factor, 2.5);
    }
  }
}

TEST(EmissionConfig, DetectsNonstandardKeys)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/emission/contains_nonstandard_key/config" + extension;
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
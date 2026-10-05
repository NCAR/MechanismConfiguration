// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "utils/print.hpp"

#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidSpecies)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = Parse(std::string("v1_unit_configs/species/valid_species") + extension);
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed;
    EXPECT_EQ(mechanism.species.size(), 3);

    EXPECT_EQ(mechanism.species[0].name, "A");
    EXPECT_EQ(mechanism.species[0].unknown_properties.size(), 2);
    EXPECT_EQ(mechanism.species[0].unknown_properties["__absolute tolerance"], "1.0e-30");
    EXPECT_EQ(mechanism.species[0].unknown_properties["__long name"], "ozone");
    EXPECT_EQ(mechanism.species[0].is_third_body.has_value(), true);
    EXPECT_EQ(mechanism.species[0].is_third_body.value(), true);

    EXPECT_EQ(mechanism.species[1].name, "H2O2");
    EXPECT_EQ(mechanism.species[1].molecular_weight.has_value(), true);
    EXPECT_EQ(mechanism.species[1].molecular_weight.value(), 0.0340147);
    EXPECT_EQ(mechanism.species[1].constant_concentration.has_value(), true);
    EXPECT_EQ(mechanism.species[1].constant_concentration.value(), 2.5e19);
    EXPECT_EQ(mechanism.species[1].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.species[1].unknown_properties["__absolute tolerance"], "1.0e-10");

    EXPECT_EQ(mechanism.species[2].name, "aerosol stuff");
    EXPECT_EQ(mechanism.species[2].molecular_weight.has_value(), true);
    EXPECT_EQ(mechanism.species[2].molecular_weight.value(), 0.5);
    EXPECT_EQ(mechanism.species[2].constant_mixing_ratio.has_value(), true);
    EXPECT_EQ(mechanism.species[2].constant_mixing_ratio.value(), 1.0e-6);
    EXPECT_EQ(mechanism.species[2].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.species[2].unknown_properties["__absolute tolerance"], "1.0e-20");
  }
}

TEST(ParserBase, DetectsDuplicateSpecies)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/species/duplicate_species") + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 4);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::DuplicateSpeciesDetected);
    EXPECT_EQ(parsed.error()[1].first, ErrorCode::DuplicateSpeciesDetected);
    EXPECT_EQ(parsed.error()[2].first, ErrorCode::DuplicateSpeciesDetected);
    EXPECT_EQ(parsed.error()[3].first, ErrorCode::DuplicateSpeciesDetected);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, DetectsMissingRequiredKeys)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/species/missing_required_key") + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 2);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::RequiredKeyNotFound);
    EXPECT_EQ(parsed.error()[1].first, ErrorCode::InvalidKey);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, DetectsInvalidKeys)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/species/invalid_key") + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::InvalidKey);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

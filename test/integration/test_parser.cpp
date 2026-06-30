// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/mechanism_configuration.hpp>

#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <string>

using namespace mechanism_configuration;

TEST(Parse, ConfigurationWithoutVersionFallsBackToV0)
{
  for (const auto& extension : { std::string(".yaml"), std::string(".json") })
  {
    auto parsed = Parse("examples/v0/config" + extension);
    EXPECT_TRUE(parsed);
    if (parsed)
      EXPECT_EQ(parsed->version.major, 0);
  }
}

TEST(Parse, ParsesFullV1Configuration)
{
  for (const auto& extension : { std::string(".json"), std::string(".yaml") })
  {
    auto parsed = Parse("examples/v1/full_configuration" + extension);
    if (!parsed)
      for (const auto& [code, message] : parsed.error())
        std::cout << message << std::endl;
    EXPECT_TRUE(parsed);
    if (parsed)
      EXPECT_EQ(parsed->version.major, 1);
  }
}

TEST(Parse, ReportsMissingFile)
{
  for (const auto& extension : { std::string(".yaml"), std::string(".json") })
  {
    auto parsed = Parse("examples/_missing_configuration" + extension);
    EXPECT_FALSE(parsed);
    ASSERT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::FileNotFound);
  }
}

TEST(Parse, ReportsUnsupportedVersion)
{
  auto parsed = Parse("integration_configs/invalid_version.yaml");
  EXPECT_FALSE(parsed);

  bool found_invalid_version = false;
  for (const auto& [code, message] : parsed.error())
  {
    if (code == ErrorCode::InvalidVersion)
      found_invalid_version = true;
    std::cout << message << " " << ErrorCodeToString(code) << std::endl;
  }
  EXPECT_TRUE(found_invalid_version);
}

TEST(Parse, ParsesV0DirectoryConfiguration)
{
  // A directory is treated as a version-0 (CAMP) configuration.
  auto parsed = Parse(std::filesystem::path("examples/v0/"));
  EXPECT_TRUE(parsed);
  if (parsed)
    EXPECT_EQ(parsed->version.major, 0);
}

TEST(Parse, ParsesV1JsonString)
{
  std::string config = R"(
  {
    "version": "1.0.0",
    "species": [
      {
        "name": "H2O",
      }
    ],
    "phases": [
      {
        "name": "gas",
        "species": [ 
          {
            "name": "H2O"
          }
        ]
      }
    ],
    "reactions": [
      {
        type: "ARRHENIUS",
        "reactants": [ { "species name": "H2O" } ],
        "products": [ { "species name": "H2O" } ],
        "gas phase": "gas",
      }
    ]
  }
  )";
  auto parsed = ParseFromString(config);
  EXPECT_TRUE(parsed);
  if (parsed){
    EXPECT_EQ(parsed->version.major, 1);
    EXPECT_EQ(parsed->species.size(), 1);
    EXPECT_EQ(parsed->phases.size(), 1);
    EXPECT_EQ(parsed->reactions.arrhenius.size(), 1);
  }
  else {
    for (const auto& [code, message] : parsed.error())
      std::cout << message << " " << ErrorCodeToString(code) << std::endl;
  }
}

TEST(Parse, ParsesV11JsonString)
{
  std::string config = R"(
  {
    "version": "1.1.0",
    "species": [
      {
        "name": "H2O",
      }
    ],
    "phases": [
      {
        "name": "gas",
        "species": [ 
          {
            "name": "H2O"
          }
        ]
      }
    ],
    "reactions": [
      {
        type: "ARRHENIUS",
        "reactants": [ { "species name": "H2O" } ],
        "products": [ { "species name": "H2O" } ],
        "gas phase": "gas",
      }
    ]
  }
  )";
  auto parsed = ParseFromString(config);
  EXPECT_TRUE(parsed);
  if (parsed){
    EXPECT_EQ(parsed->version.major, 1);
    EXPECT_EQ(parsed->species.size(), 1);
    EXPECT_EQ(parsed->phases.size(), 1);
    EXPECT_EQ(parsed->reactions.arrhenius.size(), 1);
  }
  else {
    for (const auto& [code, message] : parsed.error())
      std::cout << message << " " << ErrorCodeToString(code) << std::endl;
  }
}

TEST(Parse, ParsesV1YamlString)
{
  std::string config = R"(
version: 1.0.0
species:
  - name: H2O
phases:
  - name: gas
    species:
      - name: H2O
reactions:
  - type: ARRHENIUS
    reactants:
      - species name: H2O
    products:
      - species name: H2O
    gas phase: gas
  )";
  auto parsed = ParseFromString(config);
  EXPECT_TRUE(parsed);
  if (parsed){
    EXPECT_EQ(parsed->version.major, 1);
    EXPECT_EQ(parsed->species.size(), 1);
    EXPECT_EQ(parsed->phases.size(), 1);
    EXPECT_EQ(parsed->reactions.arrhenius.size(), 1);
  }
  else {
    for (const auto& [code, message] : parsed.error())
      std::cout << message << " " << ErrorCodeToString(code) << std::endl;
  }
}

TEST(Parse, ParsesV11YamlString)
{
  std::string config = R"(
version: 1.1.0
species:
  - name: H2O
phases:
  - name: gas
    species:
      - name: H2O
reactions:
  - type: ARRHENIUS
    reactants:
      - species name: H2O
    products:
      - species name: H2O
    gas phase: gas
  )";
  auto parsed = ParseFromString(config);
  EXPECT_TRUE(parsed);
  if (parsed){
    EXPECT_EQ(parsed->version.major, 1);
    EXPECT_EQ(parsed->species.size(), 1);
    EXPECT_EQ(parsed->phases.size(), 1);
    EXPECT_EQ(parsed->reactions.arrhenius.size(), 1);
  }
  else {
    for (const auto& [code, message] : parsed.error())
      std::cout << message << " " << ErrorCodeToString(code) << std::endl;
  }
}
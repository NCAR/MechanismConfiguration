// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "utils/print.hpp"

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

TEST(Parse, ParsesCamCloudChemistryAerosolConfiguration)
{
  auto parsed = Parse("examples/v1/cam_cloud_chemistry.json");
  if (!parsed)
    for (const auto& [code, message] : parsed.error())
      std::cout << message << std::endl;
  ASSERT_TRUE(parsed);

  const Mechanism& mechanism = *parsed;
  EXPECT_EQ(mechanism.version.major, 1);
  EXPECT_EQ(mechanism.name, "CAM Cloud Chemistry");
  EXPECT_EQ(mechanism.species.size(), 10u);
  EXPECT_EQ(mechanism.phases.size(), 2u);

  ASSERT_TRUE(mechanism.aerosol.has_value());

  // One UNIFORM_SECTION representation.
  ASSERT_EQ(mechanism.aerosol->representations.size(), 1u);
  const auto& cloud = std::get<types::UniformSection>(mechanism.aerosol->representations[0]);
  EXPECT_EQ(cloud.name, "CLOUD");

  // Processes: one reversible reaction followed by three dissolved reactions.
  ASSERT_EQ(mechanism.aerosol->processes.size(), 4u);
  const auto& reversible = std::get<types::DissolvedReversibleReaction>(mechanism.aerosol->processes[0]);
  ASSERT_EQ(reversible.reactants.size(), 2u);
  EXPECT_EQ(reversible.reactants[0].name, "HSO3m");  // components keyed on "name"
  ASSERT_TRUE(reversible.equilibrium_constant.has_value());
  EXPECT_DOUBLE_EQ(reversible.equilibrium_constant->A, 1725.0);

  // The ozone pathway: HSO3- + O3 and SO3-- + O3 both produce SO4--.
  const auto& ozone_hso3 = std::get<types::DissolvedReaction>(mechanism.aerosol->processes[2]);
  ASSERT_EQ(ozone_hso3.reactants.size(), 2u);
  EXPECT_EQ(ozone_hso3.reactants[0].name, "HSO3m");
  EXPECT_EQ(ozone_hso3.reactants[1].name, "O3");
  const auto& ozone_so3 = std::get<types::DissolvedReaction>(mechanism.aerosol->processes[3]);
  ASSERT_EQ(ozone_so3.reactants.size(), 2u);
  EXPECT_EQ(ozone_so3.reactants[0].name, "SO3mm");
  EXPECT_EQ(ozone_so3.reactants[1].name, "O3");
  ASSERT_EQ(ozone_so3.products.size(), 1u);
  EXPECT_EQ(ozone_so3.products[0].name, "SO4mm");

  // Constraints: 3 Henry's-law equilibria + 3 dissolved equilibria + 4 linear constraints.
  ASSERT_EQ(mechanism.aerosol->constraints.size(), 10u);

  // The first constraint is the SO2 Henry's-law equilibrium; its solvent properties are sourced
  // from the species/phase definitions rather than the process block.
  const auto& so2_equilibrium = std::get<types::HenrysLawEquilibrium>(mechanism.aerosol->constraints[0]);
  EXPECT_EQ(so2_equilibrium.solvent, "H2O");
  EXPECT_DOUBLE_EQ(so2_equilibrium.solvent_molecular_weight, 0.01801);  // from the species section
  EXPECT_DOUBLE_EQ(so2_equilibrium.solvent_density, 997.0);             // from the AQUEOUS phase

  // The first linear constraint's terms are keyed on "name".
  const auto& linear = std::get<types::LinearConstraint>(mechanism.aerosol->constraints[6]);
  ASSERT_FALSE(linear.terms.empty());
  EXPECT_EQ(linear.terms[0].phase, "gas");
  EXPECT_EQ(linear.terms[0].name, "SO2");
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
  if (parsed)
  {
    EXPECT_EQ(parsed->version.major, 1);
    EXPECT_EQ(parsed->species.size(), 1);
    EXPECT_EQ(parsed->phases.size(), 1);
    EXPECT_EQ(parsed->reactions.arrhenius.size(), 1);
  }
  else
  {
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
  if (parsed)
  {
    EXPECT_EQ(parsed->version.major, 1);
    EXPECT_EQ(parsed->species.size(), 1);
    EXPECT_EQ(parsed->phases.size(), 1);
    EXPECT_EQ(parsed->reactions.arrhenius.size(), 1);
  }
  else
  {
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
  if (parsed)
  {
    EXPECT_EQ(parsed->version.major, 1);
    EXPECT_EQ(parsed->species.size(), 1);
    EXPECT_EQ(parsed->phases.size(), 1);
    EXPECT_EQ(parsed->reactions.arrhenius.size(), 1);
  }
  else
  {
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
  if (parsed)
  {
    EXPECT_EQ(parsed->version.major, 1);
    EXPECT_EQ(parsed->species.size(), 1);
    EXPECT_EQ(parsed->phases.size(), 1);
    EXPECT_EQ(parsed->reactions.arrhenius.size(), 1);
  }
  else
  {
    for (const auto& [code, message] : parsed.error())
      std::cout << message << " " << ErrorCodeToString(code) << std::endl;
  }
}

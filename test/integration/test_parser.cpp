// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "utils/print.hpp"

#include <mechanism_configuration/mechanism_configuration.hpp>

#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

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

TEST(Parse, ParsesTheExampleForEachV1MinorVersion)
{
  struct Example
  {
    std::string path;
    unsigned int minor;
    bool has_aerosol;
    bool has_emissions;
  };
  const std::vector<Example> examples = {
    { "examples/v1/1.0/config.yaml", 0, false, false },      { "examples/v1/1.0/config.json", 0, false, false },
    { "examples/v1/1.1/yaml/config.yaml", 1, false, false }, { "examples/v1/1.1/json/config.json", 1, false, false },
    { "examples/v1/1.2/config.yaml", 2, true, false },       { "examples/v1/1.2/config.json", 2, true, false },
    { "examples/v1/1.3/config.yaml", 3, true, true },        { "examples/v1/1.3/config.json", 3, true, true },
  };

  for (const auto& example : examples)
  {
    auto parsed = Parse(example.path);
    if (!parsed)
      for (const auto& [code, message] : parsed.error())
        std::cout << message << std::endl;
    ASSERT_TRUE(parsed) << example.path;

    const Mechanism& mechanism = *parsed;
    EXPECT_EQ(mechanism.version.major, 1) << example.path;
    EXPECT_EQ(mechanism.version.minor, example.minor) << example.path;

    // Each example has every gas-phase reaction type.
    const auto& reactions = mechanism.reactions;
    EXPECT_EQ(reactions.arrhenius.size(), 2) << example.path;
    EXPECT_EQ(reactions.branched.size(), 1) << example.path;
    EXPECT_EQ(reactions.emission.size(), 1) << example.path;
    EXPECT_EQ(reactions.first_order_loss.size(), 1) << example.path;
    EXPECT_EQ(reactions.lambda_rate_constant.size(), 1) << example.path;
    EXPECT_EQ(reactions.photolysis.size(), 1) << example.path;
    EXPECT_EQ(reactions.surface.size(), 1) << example.path;
    EXPECT_EQ(reactions.taylor_series.size(), 1) << example.path;
    EXPECT_EQ(reactions.ternary_chemical_activation.size(), 1) << example.path;
    EXPECT_EQ(reactions.troe.size(), 1) << example.path;
    EXPECT_EQ(reactions.tunneling.size(), 1) << example.path;
    EXPECT_EQ(reactions.user_defined.size(), 1) << example.path;

    // The aerosol examples have every representation, process, and constraint type.
    EXPECT_EQ(mechanism.aerosol.has_value(), example.has_aerosol) << example.path;
    if (mechanism.aerosol)
    {
      EXPECT_EQ(mechanism.aerosol->representations.size(), 3) << example.path;
      EXPECT_EQ(mechanism.aerosol->processes.size(), 3) << example.path;
      EXPECT_EQ(mechanism.aerosol->constraints.size(), 3) << example.path;
    }

    EXPECT_EQ(mechanism.emissions.has_value(), example.has_emissions) << example.path;
    if (mechanism.emissions)
      EXPECT_EQ(mechanism.emissions->sources.size(), 1) << example.path;
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

static void ExpectCamCloudChemistry(const std::string& path)
{
  SCOPED_TRACE(path);
  auto parsed = Parse(path);
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

TEST(Parse, ParsesCamCloudChemistryAerosolConfiguration)
{
  ExpectCamCloudChemistry("examples/v1/cam_cloud_chemistry.json");
  ExpectCamCloudChemistry("examples/v1/cam_cloud_chemistry.yaml");
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

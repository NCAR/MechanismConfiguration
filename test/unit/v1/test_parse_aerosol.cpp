// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <variant>

using namespace mechanism_configuration;

namespace
{
  bool HasError(const Errors& errors, ErrorCode code)
  {
    for (const auto& [error_code, message] : errors)
    {
      std::cout << message << " " << ErrorCodeToString(error_code) << std::endl;
      if (error_code == code)
        return true;
    }
    return false;
  }
}  // namespace

TEST(ParseAerosol, ParsesValidAerosolConfiguration)
{
  auto parsed = Parse("v1_unit_configs/aerosol/valid_aerosol.json");
  if (!parsed)
    for (const auto& [code, message] : parsed.error())
      std::cout << message << std::endl;
  ASSERT_TRUE(parsed);

  const Mechanism& mechanism = *parsed;
  const auto& aerosol = mechanism.aerosol;

  // Two SINGLE_MOMENT_MODE representations.
  ASSERT_EQ(aerosol.representations.size(), 2u);
  const auto& aitken = std::get<types::SingleMomentMode>(aerosol.representations[0]);
  EXPECT_EQ(aitken.name, "aitken");
  EXPECT_DOUBLE_EQ(aitken.geometric_mean_radius, 1.0e-6);
  EXPECT_DOUBLE_EQ(aitken.geometric_standard_deviation, 1.0e-5);
  const auto& accumulation = std::get<types::SingleMomentMode>(aerosol.representations[1]);
  EXPECT_EQ(accumulation.name, "accumulation");
  EXPECT_DOUBLE_EQ(accumulation.geometric_mean_radius, 2.0e-5);
  EXPECT_DOUBLE_EQ(accumulation.geometric_standard_deviation, 1.0e-5);

  // Processes: a phase transfer followed by a dissolved reaction.
  ASSERT_EQ(aerosol.processes.size(), 2u);

  // The phase transfer's diffusion coefficient is sourced from the gas-phase species definition.
  const auto& transfer = std::get<types::HenryLawPhaseTransfer>(aerosol.processes[0]);
  EXPECT_DOUBLE_EQ(transfer.diffusion_coefficient, 1.5e-5);
  EXPECT_DOUBLE_EQ(transfer.accommodation_coefficient, 0.1);

  // The dissolved reaction's components are keyed on "name".
  const auto& reaction = std::get<types::DissolvedReaction>(aerosol.processes[1]);
  ASSERT_EQ(reaction.reactants.size(), 1u);
  ASSERT_EQ(reaction.products.size(), 1u);
  EXPECT_EQ(reaction.reactants[0].name, "A");
  EXPECT_EQ(reaction.products[0].name, "B");

  // Constraints: a Henry's-law equilibrium followed by a linear constraint.
  ASSERT_EQ(aerosol.constraints.size(), 2u);

  // The solvent properties are sourced from the species/phase definitions.
  const auto& equilibrium = std::get<types::HenryLawEquilibrium>(aerosol.constraints[0]);
  EXPECT_EQ(equilibrium.solvent, "H2O");
  EXPECT_DOUBLE_EQ(equilibrium.solvent_molecular_weight, 0.018);  // from the species section
  EXPECT_DOUBLE_EQ(equilibrium.solvent_density, 1000.0);          // from the aqueous phase

  // The linear constraint's terms are keyed on "name".
  const auto& linear = std::get<types::LinearConstraint>(aerosol.constraints[1]);
  ASSERT_EQ(linear.terms.size(), 1u);
  EXPECT_EQ(linear.terms[0].phase, "gas");
  EXPECT_EQ(linear.terms[0].name, "A");
}

TEST(ParseAerosol, RejectsPhaseTransferWhenGasSpeciesHasNoDiffusionCoefficient)
{
  auto parsed = Parse("v1_unit_configs/aerosol/phase_transfer_missing_diffusion.json");
  EXPECT_FALSE(parsed);
  ASSERT_FALSE(parsed) << "Expected validation to fail for a missing diffusion coefficient.";
  EXPECT_TRUE(HasError(parsed.error(), ErrorCode::RequiredKeyNotFound));
}

TEST(ParseAerosol, RejectsHenryLawEquilibriumWhenSolventHasNoDensity)
{
  auto parsed = Parse("v1_unit_configs/aerosol/equilibrium_missing_solvent_density.json");
  EXPECT_FALSE(parsed);
  ASSERT_FALSE(parsed) << "Expected validation to fail for a missing solvent density.";
  EXPECT_TRUE(HasError(parsed.error(), ErrorCode::RequiredKeyNotFound));
}

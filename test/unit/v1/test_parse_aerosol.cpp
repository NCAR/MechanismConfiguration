// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "utils/print.hpp"

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

  bool HasErrorAt(const Errors& errors, ErrorCode code, const std::string& location)
  {
    for (const auto& [error_code, message] : errors)
    {
      std::cout << message << " " << ErrorCodeToString(error_code) << std::endl;
      if (error_code == code && message.find(location + " error:") != std::string::npos)
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
  ASSERT_TRUE(mechanism.aerosol.has_value());
  const auto& aerosol = *mechanism.aerosol;

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
  const auto& transfer = std::get<types::HenrysLawPhaseTransfer>(aerosol.processes[0]);
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
  const auto& equilibrium = std::get<types::HenrysLawEquilibrium>(aerosol.constraints[0]);
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
  // The location should point at the gas-phase species reference (line 28), not just anywhere.
  EXPECT_TRUE(HasErrorAt(parsed.error(), ErrorCode::RequiredKeyNotFound, "28:28"));
}

TEST(ParseAerosol, RejectsHenrysLawEquilibriumWhenSolventHasNoDensity)
{
  auto parsed = Parse("v1_unit_configs/aerosol/equilibrium_missing_solvent_density.json");
  EXPECT_FALSE(parsed);
  ASSERT_FALSE(parsed) << "Expected validation to fail for a missing solvent density.";
  EXPECT_TRUE(HasError(parsed.error(), ErrorCode::RequiredKeyNotFound));
  // The location should point at the solvent reference (line 31), not just anywhere.
  EXPECT_TRUE(HasErrorAt(parsed.error(), ErrorCode::RequiredKeyNotFound, "31:18"));
}

TEST(ParseAerosol, RejectsRepresentationReferencingUnknownPhase)
{
  auto parsed = Parse("v1_unit_configs/aerosol/representation_unknown_phase.json");
  ASSERT_FALSE(parsed) << "Expected validation to fail for a representation referencing an unknown phase.";
  // Location points at the offending phase name inside the representation's "phases" list (line 14).
  EXPECT_TRUE(HasErrorAt(parsed.error(), ErrorCode::UnknownPhase, "14:18"));
}

TEST(ParseAerosol, RejectsDissolvedReactionReferencingUnknownPhase)
{
  auto parsed = Parse("v1_unit_configs/aerosol/dissolved_reaction_unknown_phase.json");
  ASSERT_FALSE(parsed) << "Expected validation to fail for a dissolved reaction referencing an unknown phase.";
  // Location points at the "condensed phase" value (line 22), reused for reactant/product/solvent checks.
  EXPECT_TRUE(HasErrorAt(parsed.error(), ErrorCode::UnknownPhase, "22:26"));
}

TEST(ParseAerosol, RejectsDissolvedReactionReactantNotRegisteredInPhase)
{
  auto parsed = Parse("v1_unit_configs/aerosol/dissolved_reaction_reactant_not_in_phase.json");
  ASSERT_FALSE(parsed) << "Expected validation to fail for a reactant not registered in the reaction's phase.";
  // Location points at the reactant entry itself (line 25), not the phase or the reaction as a whole.
  EXPECT_TRUE(HasErrorAt(parsed.error(), ErrorCode::RequestedSpeciesNotRegisteredInPhase, "25:22"));
}

TEST(ParseAerosol, RejectsHenrysLawEquilibriumReferencingUnknownSolventSpecies)
{
  auto parsed = Parse("v1_unit_configs/aerosol/henrys_law_equilibrium_unknown_solvent_species.json");
  ASSERT_FALSE(parsed) << "Expected validation to fail for a solvent species that isn't declared anywhere.";
  // Location points at the "solvent" value (line 27). The undeclared species trips both the
  // phase-membership check (density) and the species-existence check (molecular weight).
  EXPECT_TRUE(HasErrorAt(parsed.error(), ErrorCode::RequestedSpeciesNotRegisteredInPhase, "27:18"));
  EXPECT_TRUE(HasErrorAt(parsed.error(), ErrorCode::UnknownSpecies, "27:18"));
}

TEST(ParseAerosol, RejectsLinearConstraintTermReferencingUnknownPhase)
{
  auto parsed = Parse("v1_unit_configs/aerosol/linear_constraint_term_unknown_phase.json");
  ASSERT_FALSE(parsed) << "Expected validation to fail for a linear constraint term referencing an unknown phase.";
  // Location points at the term's own "phase" value (line 25), nested inside the terms list.
  EXPECT_TRUE(HasErrorAt(parsed.error(), ErrorCode::UnknownPhase, "25:29"));
}

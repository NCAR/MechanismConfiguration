// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

namespace mechanism_configuration::v1::keys
{
  // ----------------------------------------
  // Top-level containers
  // ----------------------------------------
  inline constexpr std::string_view aerosol_representations = "aerosol representations";
  inline constexpr std::string_view aerosol_processes = "aerosol processes";

  // ----------------------------------------
  // Representations
  // ----------------------------------------
  // also: name, phases

  inline constexpr std::string_view UniformSection_key = "UNIFORM_SECTION";
  inline constexpr std::string_view minimum_radius = "minimum radius [m]";
  inline constexpr std::string_view maximum_radius = "maximum radius [m]";

  inline constexpr std::string_view SingleMomentMode_key = "SINGLE_MOMENT_MODE";
  inline constexpr std::string_view geometric_mean_radius = "geometric mean radius [m]";
  inline constexpr std::string_view geometric_standard_deviation = "geometric standard deviation";

  inline constexpr std::string_view TwoMomentMode_key = "TWO_MOMENT_MODE";
  // also: geometric_standard_deviation

  // ----------------------------------------
  // Rate constants
  // ----------------------------------------
  inline constexpr std::string_view rate_constants = "rate constants";
  inline constexpr std::string_view forward_rate_constants = "forward rate constants";
  inline constexpr std::string_view reverse_rate_constants = "reverse rate constants";
  inline constexpr std::string_view equilibrium_constant = "equilibrium constant";
  inline constexpr std::string_view reference_temperature = "T0 [K]";

  inline constexpr std::string_view ArrheniusReferenceTemperature_key = "ARRHENIUS_REFERENCE_TEMPERATURE";

  inline constexpr std::string_view henry_law_constant = "Henry's law constant";
  inline constexpr std::string_view HLC_ref = "HLC_ref [mol m-3 Pa-1]";
  inline constexpr std::string_view henry_law_C = "C [K]";

  // ----------------------------------------
  // Processes
  // ----------------------------------------
  inline constexpr std::string_view solvent = "solvent";
  inline constexpr std::string_view condensed_phase_species = "condensed-phase species";

  // HenryLawPhaseTransfer
  // also: gas_phase, gas_phase_species, condensed_phase, condensed_phase_species, solvent,
  //       henry_law_constant, diffusion_coefficient
  inline constexpr std::string_view HenryLawPhaseTransfer_key = "HENRY_LAW_PHASE_TRANSFER";
  inline constexpr std::string_view accommodation_coefficient = "accommodation coefficient";

  // DissolvedReaction
  // also: condensed_phase, solvent, reactants, products, rate_constants
  inline constexpr std::string_view DissolvedReaction_key = "DISSOLVED_REACTION";

  // DissolvedReversibleReaction
  // also: condensed_phase, solvent, reactants, products,
  //       forward_rate_constants, reverse_rate_constants, equilibrium_constant
  inline constexpr std::string_view DissolvedReversibleReaction_key = "DISSOLVED_REVERSIBLE_REACTION";

  // ----------------------------------------
  // Constraints
  // ----------------------------------------
  // HenryLawEquilibrium
  // also: gas_phase, gas_phase_species, condensed_phase, condensed_phase_species, solvent,
  //       henry_law_constant
  inline constexpr std::string_view HenryLawEquilibrium_key = "HENRY_LAW_EQUILIBRIUM";
  inline constexpr std::string_view solvent_molecular_weight = "solvent molecular weight [kg mol-1]";
  inline constexpr std::string_view solvent_density = "solvent density [kg m-3]";

  // DissolvedEquilibrium
  // also: condensed_phase, solvent, reactants, products, equilibrium_constant
  inline constexpr std::string_view DissolvedEquilibrium_key = "DISSOLVED_EQUILIBRIUM";
  inline constexpr std::string_view algebraic_species = "algebraic species";

  // LinearConstraint
  // also: name; terms carry coefficient and name
  inline constexpr std::string_view LinearConstraint_key = "LINEAR_CONSTRAINT";
  inline constexpr std::string_view algebraic_phase = "algebraic phase";
  inline constexpr std::string_view terms = "terms";
  inline constexpr std::string_view phase = "phase";
  inline constexpr std::string_view constant = "constant [mol m-3]";
  inline constexpr std::string_view diagnose_from_state = "diagnose from state";

}  // namespace mechanism_configuration::v1::keys

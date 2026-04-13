// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mechanism_configuration
{
  namespace v1
  {
    namespace validation
    {
      // Shared, but also Mechanism
      static constexpr const char* version = "version";
      static constexpr const char* name = "name";

      // Configuration
      static constexpr const char* species = "species";
      static constexpr const char* phases = "phases";
      static constexpr const char* reactions = "reactions";

      // Species
      static constexpr const char* molecular_weight = "molecular weight [kg mol-1]";
      static constexpr const char* constant_concentration = "constant concentration [mol m-3]";
      static constexpr const char* constant_mixing_ratio = "constant mixing ratio [mol mol-1]";
      static constexpr const char* is_third_body = "is third body";

      // Phase-species
      static constexpr const char* diffusion_coefficient = "diffusion coefficient [m2 s-1]";

      // Reactions
      static constexpr const char* reactants = "reactants";
      static constexpr const char* products = "products";
      static constexpr const char* type = "type";
      static constexpr const char* gas_phase = "gas phase";

      // Reactant and product
      static constexpr const char* species_name = "species name";
      static constexpr const char* coefficient = "coefficient";

      // ----------------------------------------
      // Reaction types
      // ----------------------------------------

      // Arrhenius
      static constexpr const char* Arrhenius_key = "ARRHENIUS";
      static constexpr const char* A = "A";
      static constexpr const char* B = "B";
      static constexpr const char* C = "C";
      static constexpr const char* D = "D";
      static constexpr const char* E = "E";
      static constexpr const char* Ea = "Ea";

      // TaylorSeries
      static constexpr const char* TaylorSeries_key = "TAYLOR_SERIES";
      static constexpr const char* taylor_coefficients = "taylor coefficients";
      // also these
      // A
      // B
      // C
      // D
      // E
      // Ea

      // Troe
      static constexpr const char* Troe_key = "TROE";
      static constexpr const char* k0_A = "k0_A";
      static constexpr const char* k0_B = "k0_B";
      static constexpr const char* k0_C = "k0_C";
      static constexpr const char* kinf_A = "kinf_A";
      static constexpr const char* kinf_B = "kinf_B";
      static constexpr const char* kinf_C = "kinf_C";
      static constexpr const char* Fc = "Fc";
      static constexpr const char* N = "N";

      // Ternary Chemical Activation
      static constexpr const char* TernaryChemicalActivation_key = "TERNARY_CHEMICAL_ACTIVATION";
      // also k0_A
      // k0_B
      // k0_C
      // kinf_A
      // kinf_B
      // kinf_C
      // Fc
      // N

      // Branched
      static constexpr const char* Branched_key = "BRANCHED_NO_RO2";
      static constexpr const char* X = "X";
      static constexpr const char* Y = "Y";
      static constexpr const char* a0 = "a0";
      static constexpr const char* n = "n";
      static constexpr const char* nitrate_products = "nitrate products";
      static constexpr const char* alkoxy_products = "alkoxy products";

      // Tunneling
      static constexpr const char* Tunneling_key = "TUNNELING";
      // also these, but they are defined above
      // A
      // B
      // C

      // Surface
      static constexpr const char* Surface_key = "SURFACE";
      static constexpr const char* reaction_probability = "reaction probability";
      static constexpr const char* gas_phase_species = "gas-phase species";
      static constexpr const char* gas_phase_products = "gas-phase products";

      // Photolysis
      static constexpr const char* Photolysis_key = "PHOTOLYSIS";
      static constexpr const char* scaling_factor = "scaling factor";

      // Emissions
      static constexpr const char* Emission_key = "EMISSION";
      // also scaling factor

      // First Order Loss
      static constexpr const char* FirstOrderLoss_key = "FIRST_ORDER_LOSS";
      // also scaling factor

      // User Defined
      static constexpr const char* UserDefined_key = "USER_DEFINED";
      // also
      // gas phase
      // reactants
      // products
      // scaling factor

      // Lambda Rate Constant
      static constexpr const char* LambdaRateConstant_key = "LAMBDA_RATE_CONSTANT";
      static constexpr const char* lambda_function = "lambda function";
      // also
      // gas phase
      // reactants
      // products
      // name

      // ----------------------------------------
      // Aerosol representations
      // ----------------------------------------
      static constexpr const char* aerosol_representations = "aerosol representations";
      static constexpr const char* aerosol_processes = "aerosol processes";

      // Representation types
      static constexpr const char* UniformSection_key = "UNIFORM_SECTION";
      static constexpr const char* SingleMomentMode_key = "SINGLE_MOMENT_MODE";
      static constexpr const char* TwoMomentMode_key = "TWO_MOMENT_MODE";
      static constexpr const char* minimum_radius = "minimum radius [m]";
      static constexpr const char* maximum_radius = "maximum radius [m]";
      static constexpr const char* geometric_mean_radius = "geometric mean radius [m]";
      static constexpr const char* geometric_standard_deviation = "geometric standard deviation";

      // ----------------------------------------
      // Aerosol process types
      // ----------------------------------------
      static constexpr const char* HenryLawEquilibrium_key = "HENRY_LAW_EQUILIBRIUM";
      static constexpr const char* HenryLawPhaseTransfer_key = "HENRY_LAW_PHASE_TRANSFER";
      static constexpr const char* DissolvedReaction_key = "DISSOLVED_REACTION";
      static constexpr const char* DissolvedReversibleReaction_key = "DISSOLVED_REVERSIBLE_REACTION";
      static constexpr const char* DissolvedEquilibrium_key = "DISSOLVED_EQUILIBRIUM";
      static constexpr const char* LinearConstraint_key = "LINEAR_CONSTRAINT";

      // Aerosol process keys
      static constexpr const char* condensed_phase = "condensed phase";
      static constexpr const char* condensed_phase_species = "condensed-phase species";

      static constexpr const char* solvent = "solvent";
      static constexpr const char* henrys_law_constant = "Henry's law constant";
      static constexpr const char* hlc_ref = "HLC_ref [mol m-3 Pa-1]";
      static constexpr const char* rate_constant = "rate constant";
      static constexpr const char* forward_rate_constant = "forward rate constant";
      static constexpr const char* reverse_rate_constant = "reverse rate constant";
      static constexpr const char* equilibrium_constant = "equilibrium constant";
      static constexpr const char* algebraic_species = "algebraic species";
      static constexpr const char* algebraic_phase = "algebraic phase";
      static constexpr const char* diagnose_from_state = "diagnose from state";
      static constexpr const char* terms = "terms";
      static constexpr const char* phase_key = "phase";
      static constexpr const char* constant_mol_m3 = "constant [mol m-3]";
      static constexpr const char* aerosol_accommodation_coefficient = "accommodation coefficient";
      static constexpr const char* density = "density [kg m-3]";

    }  // namespace validation
  }  // namespace v1
}  // namespace mechanism_configuration
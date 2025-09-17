// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
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

    }  // namespace validation
  }  // namespace v1
}  // namespace mechanism_configuration
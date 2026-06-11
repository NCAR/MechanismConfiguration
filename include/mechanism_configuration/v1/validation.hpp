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
      inline constexpr std::string_view version = "version";
      inline constexpr std::string_view name = "name";

      // Configuration
      inline constexpr std::string_view species = "species";
      inline constexpr std::string_view phases = "phases";
      inline constexpr std::string_view reactions = "reactions";

      // Species
      inline constexpr std::string_view molecular_weight = "molecular weight [kg mol-1]";
      inline constexpr std::string_view constant_concentration = "constant concentration [mol m-3]";
      inline constexpr std::string_view constant_mixing_ratio = "constant mixing ratio [mol mol-1]";
      inline constexpr std::string_view is_third_body = "is third body";

      // Phase-species
      inline constexpr std::string_view diffusion_coefficient = "diffusion coefficient [m2 s-1]";

      // Reactions
      inline constexpr std::string_view reactants = "reactants";
      inline constexpr std::string_view products = "products";
      inline constexpr std::string_view type = "type";
      inline constexpr std::string_view gas_phase = "gas phase";

      // Reactant and product
      inline constexpr std::string_view species_name = "species name";
      inline constexpr std::string_view coefficient = "coefficient";

      // ----------------------------------------
      // Reaction types
      // ----------------------------------------

      // Arrhenius
      inline constexpr std::string_view Arrhenius_key = "ARRHENIUS";
      inline constexpr std::string_view A = "A";
      inline constexpr std::string_view B = "B";
      inline constexpr std::string_view C = "C";
      inline constexpr std::string_view D = "D";
      inline constexpr std::string_view E = "E";
      inline constexpr std::string_view Ea = "Ea";

      // TaylorSeries
      inline constexpr std::string_view TaylorSeries_key = "TAYLOR_SERIES";
      inline constexpr std::string_view taylor_coefficients = "taylor coefficients";
      // also these
      // A
      // B
      // C
      // D
      // E
      // Ea

      // Troe
      inline constexpr std::string_view Troe_key = "TROE";
      inline constexpr std::string_view k0_A = "k0_A";
      inline constexpr std::string_view k0_B = "k0_B";
      inline constexpr std::string_view k0_C = "k0_C";
      inline constexpr std::string_view kinf_A = "kinf_A";
      inline constexpr std::string_view kinf_B = "kinf_B";
      inline constexpr std::string_view kinf_C = "kinf_C";
      inline constexpr std::string_view Fc = "Fc";
      inline constexpr std::string_view N = "N";

      // Ternary Chemical Activation
      inline constexpr std::string_view TernaryChemicalActivation_key = "TERNARY_CHEMICAL_ACTIVATION";
      // also k0_A
      // k0_B
      // k0_C
      // kinf_A
      // kinf_B
      // kinf_C
      // Fc
      // N

      // Branched
      inline constexpr std::string_view Branched_key = "BRANCHED_NO_RO2";
      inline constexpr std::string_view X = "X";
      inline constexpr std::string_view Y = "Y";
      inline constexpr std::string_view a0 = "a0";
      inline constexpr std::string_view n = "n";
      inline constexpr std::string_view nitrate_products = "nitrate products";
      inline constexpr std::string_view alkoxy_products = "alkoxy products";

      // Tunneling
      inline constexpr std::string_view Tunneling_key = "TUNNELING";
      // also these, but they are defined above
      // A
      // B
      // C

      // Surface
      inline constexpr std::string_view Surface_key = "SURFACE";
      inline constexpr std::string_view reaction_probability = "reaction probability";
      inline constexpr std::string_view gas_phase_species = "gas-phase species";
      inline constexpr std::string_view gas_phase_products = "gas-phase products";

      // Photolysis
      inline constexpr std::string_view Photolysis_key = "PHOTOLYSIS";
      inline constexpr std::string_view scaling_factor = "scaling factor";

      // Emissions
      inline constexpr std::string_view Emission_key = "EMISSION";
      // also scaling factor

      // First Order Loss
      inline constexpr std::string_view FirstOrderLoss_key = "FIRST_ORDER_LOSS";
      // also scaling factor

      // User Defined
      inline constexpr std::string_view UserDefined_key = "USER_DEFINED";
      // also
      // gas phase
      // reactants
      // products
      // scaling factor

      // Lambda Rate Constant
      inline constexpr std::string_view LambdaRateConstant_key = "LAMBDA_RATE_CONSTANT";
      inline constexpr std::string_view lambda_function = "lambda function";
      // also
      // gas phase
      // reactants
      // products
      // name

    }  // namespace validation
  }  // namespace v1
}  // namespace mechanism_configuration
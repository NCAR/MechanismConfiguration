// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

namespace mechanism_configuration::v1::keys
{
  // Arrhenius
  inline constexpr std::string_view Arrhenius_key = "ARRHENIUS";
  // also A, B, C, D, E, Ea (shared, see detail/v1/keys.hpp)

  // TaylorSeries
  inline constexpr std::string_view TaylorSeries_key = "TAYLOR_SERIES";
  inline constexpr std::string_view taylor_coefficients = "taylor coefficients";
  // also A, B, C, D, E, Ea (shared)

  // Condensed Phase Arrhenius
  inline constexpr std::string_view CondensedPhaseArrhenius_key = "CONDENSED_PHASE_ARRHENIUS";
  // also condensed phase (shared), A, B, C, D, E, Ea (shared)

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
  // also these, but they are defined above (shared)
  // A
  // B
  // C

  // Surface
  inline constexpr std::string_view Surface_key = "SURFACE";
  inline constexpr std::string_view reaction_probability = "reaction probability";
  inline constexpr std::string_view gas_phase_products = "gas-phase products";
  // also gas_phase_species, condensed_phase (shared)

  // Photolysis
  inline constexpr std::string_view Photolysis_key = "PHOTOLYSIS";
  // also scaling factor (shared)

  // Condensed Phase Photolysis
  inline constexpr std::string_view CondensedPhasePhotolysis_key = "CONDENSED_PHASE_PHOTOLYSIS";
  // also
  // scaling factor (shared)
  // condensed phase (shared)

  // Emission
  inline constexpr std::string_view Emission_key = "EMISSION";
  // also scaling factor (shared)

  // First Order Loss
  inline constexpr std::string_view FirstOrderLoss_key = "FIRST_ORDER_LOSS";
  // also scaling factor (shared)

  // Simpol Phase Transfer
  inline constexpr std::string_view SimpolPhaseTransfer_key = "SIMPOL_PHASE_TRANSFER";
  // also
  // gas phase (shared)
  // gas-phase species (shared)
  // condensed phase (shared)
  // condensed-phase species (see detail/v1/aerosol/keys.hpp)
  // B (shared)

  // Wet Deposition
  inline constexpr std::string_view WetDeposition_key = "WET_DEPOSITION";
  // also
  // scaling factor (shared)
  // condensed phase (shared)

  // User Defined
  inline constexpr std::string_view UserDefined_key = "USER_DEFINED";
  // also
  // gas phase (shared)
  // reactants (shared)
  // products (shared)
  // scaling factor (shared)

  // Lambda Rate Constant
  inline constexpr std::string_view LambdaRateConstant_key = "LAMBDA_RATE_CONSTANT";
  inline constexpr std::string_view lambda_function = "lambda function";
  // also
  // gas phase (shared)
  // reactants (shared)
  // products (shared)
  // name (shared)

}  // namespace mechanism_configuration::v1::keys

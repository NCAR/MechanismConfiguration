// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

namespace mechanism_configuration::v1::keys
{
  // Shared, but also Mechanism
  inline constexpr std::string_view version = "version";
  inline constexpr std::string_view name = "name";
  // Legacy alias for a reaction component's species reference, accepted alongside `name`
  // for backward compatibility with v1 configuration files.
  inline constexpr std::string_view species_name = "species name";

  // Configuration
  inline constexpr std::string_view species = "species";
  inline constexpr std::string_view phases = "phases";
  inline constexpr std::string_view models = "models";
  inline constexpr std::string_view reactions = "reactions";

  // Species
  inline constexpr std::string_view absolute_tolerance = "absolute tolerance";
  inline constexpr std::string_view diffusion_coefficient = "diffusion coefficient [m2 s-1]";
  inline constexpr std::string_view molecular_weight = "molecular weight [kg mol-1]";
  inline constexpr std::string_view henrys_law_constant_298 = "HLC(298K) [mol m-3 Pa-1]";
  inline constexpr std::string_view henrys_law_constant_exponential_factor = "HLC exponential factor [K]";
  inline constexpr std::string_view phase = "phase";
  inline constexpr std::string_view n_star = "N star";
  inline constexpr std::string_view density = "density [kg m-3]";
  inline constexpr std::string_view tracer_type = "tracer type";
  inline constexpr std::string_view constant_concentration = "constant concentration [mol m-3]";
  inline constexpr std::string_view constant_mixing_ratio = "constant mixing ratio [mol mol-1]";
  inline constexpr std::string_view is_third_body = "is third body";
  inline constexpr std::string_view third_body = "THIRD_BODY";

  // Reactions
  inline constexpr std::string_view reactants = "reactants";
  inline constexpr std::string_view products = "products";
  inline constexpr std::string_view type = "type";
  inline constexpr std::string_view gas_phase = "gas phase";

  // Reactant and product
  inline constexpr std::string_view coefficient = "coefficient";
  // also name

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

  // Condensed Phase Arrhenius
  inline constexpr std::string_view CondensedPhaseArrhenius_key = "CONDENSED_PHASE_ARRHENIUS";
  // also these
  // condensed phase
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
  inline constexpr std::string_view condensed_phase = "condensed phase";

  // Photolysis
  inline constexpr std::string_view Photolysis_key = "PHOTOLYSIS";
  inline constexpr std::string_view scaling_factor = "scaling factor";

  // Condensed Phae Photolysis
  inline constexpr std::string_view CondensedPhasePhotolysis_key = "CONDENSED_PHASE_PHOTOLYSIS";
  // also
  // scaling factor
  // condensed phase

  // Emissions
  inline constexpr std::string_view Emission_key = "EMISSION";
  // also scaling factor

  // First Order Loss
  inline constexpr std::string_view FirstOrderLoss_key = "FIRST_ORDER_LOSS";
  // also scaling factor

  // Simpol Phase Transfer
  inline constexpr std::string_view SimpolPhaseTransfer_key = "SIMPOL_PHASE_TRANSFER";
  inline constexpr std::string_view condensed_phase_species = "condensed-phase species";
  // also
  // gas phase
  // gas-phase species
  // condensed phase
  // condensed-phase species
  // B

  // Wet Deposition
  inline constexpr std::string_view WetDeposition_key = "WET_DEPOSITION";
  // also
  // scaling factor
  // condensed phase

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

}  // namespace mechanism_configuration::v1::keys

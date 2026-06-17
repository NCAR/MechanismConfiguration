// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>

namespace mechanism_configuration::v0::keys
{
  inline constexpr std::string_view NAME = "name";
  inline constexpr std::string_view TYPE = "type";

  inline constexpr std::string_view VALUE = "value";

  inline constexpr std::string_view REACTIONS = "reactions";

  inline constexpr std::string_view TRACER_TYPE = "tracer type";
  inline constexpr std::string_view ABS_TOLERANCE = "absolute tolerance";
  inline constexpr std::string_view DIFFUSION_COEFF = "diffusion coefficient [m2 s-1]";
  inline constexpr std::string_view MOL_WEIGHT = "molecular weight [kg mol-1]";
  inline constexpr std::string_view THIRD_BODY = "THIRD_BODY";

  inline constexpr std::string_view REACTANTS = "reactants";
  inline constexpr std::string_view PRODUCTS = "products";
  inline constexpr std::string_view MUSICA_NAME = "MUSICA name";
  inline constexpr std::string_view SCALING_FACTOR = "scaling factor";
  inline constexpr std::string_view GAS_PHASE_REACTANT = "gas-phase reactant";
  inline constexpr std::string_view GAS_PHASE_PRODUCTS = "gas-phase products";

  inline constexpr std::string_view QTY = "qty";
  inline constexpr std::string_view YIELD = "yield";

  inline constexpr std::string_view SPECIES = "species";

  inline constexpr std::string_view ALKOXY_PRODUCTS = "alkoxy products";
  inline constexpr std::string_view NITRATE_PRODUCTS = "nitrate products";
  inline constexpr std::string_view X = "X";
  inline constexpr std::string_view Y = "Y";
  inline constexpr std::string_view A0 = "a0";
  inline constexpr std::string_view N = "N";
  inline constexpr std::string_view n = "n";

  inline constexpr std::string_view PROBABILITY = "reaction probability";

  inline constexpr std::string_view A = "A";
  inline constexpr std::string_view B = "B";
  inline constexpr std::string_view C = "C";
  inline constexpr std::string_view D = "D";
  inline constexpr std::string_view E = "E";
  inline constexpr std::string_view Ea = "Ea";

  inline constexpr std::string_view K0_A = "k0_A";
  inline constexpr std::string_view K0_B = "k0_B";
  inline constexpr std::string_view K0_C = "k0_C";
  inline constexpr std::string_view KINF_A = "kinf_A";
  inline constexpr std::string_view KINF_B = "kinf_B";
  inline constexpr std::string_view KINF_C = "kinf_C";
  inline constexpr std::string_view FC = "Fc";
}  // namespace mechanism_configuration::v0::keys
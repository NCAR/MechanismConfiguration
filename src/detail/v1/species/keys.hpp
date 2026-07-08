// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

namespace mechanism_configuration::v1::keys
{
  // Species
  inline constexpr std::string_view absolute_tolerance = "absolute tolerance";
  inline constexpr std::string_view diffusion_coefficient = "diffusion coefficient [m2 s-1]";
  inline constexpr std::string_view molecular_weight = "molecular weight [kg mol-1]";
  inline constexpr std::string_view henrys_law_constant_298 = "HLC(298K) [mol m-3 Pa-1]";
  inline constexpr std::string_view henrys_law_constant_exponential_factor = "HLC exponential factor [K]";
  inline constexpr std::string_view n_star = "N star";
  inline constexpr std::string_view density = "density [kg m-3]";
  inline constexpr std::string_view tracer_type = "tracer type";
  inline constexpr std::string_view constant_concentration = "constant concentration [mol m-3]";
  inline constexpr std::string_view constant_mixing_ratio = "constant mixing ratio [mol mol-1]";
  inline constexpr std::string_view is_third_body = "is third body";
  inline constexpr std::string_view third_body = "THIRD_BODY";

}  // namespace mechanism_configuration::v1::keys

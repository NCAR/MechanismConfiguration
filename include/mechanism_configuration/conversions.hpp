// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/constants.hpp>

namespace mechanism_configuration
{
  namespace conversions
  {
    constexpr double MolesM3ToMoleculesCm3 = 1.0e-6 * constants::avogadro;
  }  // namespace conversions
}  // namespace mechanism_configuration
// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <detail/constants.hpp>

namespace mechanism_configuration
{
  namespace conversions
  {
    constexpr double MOLES_M3_TO_MOLECULES_CM3 = 1.0e-6 * constants::AVOGADRO;
  }  // namespace conversions
}  // namespace mechanism_configuration
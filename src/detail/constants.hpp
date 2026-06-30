// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mechanism_configuration
{
  namespace constants
  {
    static constexpr double boltzmann = 1.380649e-23;  // J K^{-1}
    static constexpr double avogadro = 6.02214076e23;  // # mol^{-1}
    static constexpr double R = boltzmann * avogadro;  // J K^{-1} mol^{-1}
  }  // namespace constants
}  // namespace mechanism_configuration
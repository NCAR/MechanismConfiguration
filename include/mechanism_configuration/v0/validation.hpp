// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>

namespace mechanism_configuration
{
  namespace v0
  {
    namespace validation
    {
      static constexpr const char* NAME = "name";
      static constexpr const char* TYPE = "type";

      static constexpr const char* VALUE = "value";

      static constexpr const char* REACTIONS = "reactions";

      static constexpr const char* TRACER_TYPE = "tracer type";
      static constexpr const char* ABS_TOLERANCE = "absolute tolerance";
      static constexpr const char* DIFFUSION_COEFF = "diffusion coefficient [m2 s-1]";
      static constexpr const char* MOL_WEIGHT = "molecular weight [kg mol-1]";
      static constexpr const char* THIRD_BODY = "THIRD_BODY";

      static constexpr const char* REACTANTS = "reactants";
      static constexpr const char* PRODUCTS = "products";
      static constexpr const char* MUSICA_NAME = "MUSICA name";
      static constexpr const char* SCALING_FACTOR = "scaling factor";
      static constexpr const char* GAS_PHASE_REACTANT = "gas-phase reactant";
      static constexpr const char* GAS_PHASE_PRODUCTS = "gas-phase products";

      static constexpr const char* QTY = "qty";
      static constexpr const char* YIELD = "yield";

      static constexpr const char* SPECIES = "species";

      static constexpr const char* ALKOXY_PRODUCTS = "alkoxy products";
      static constexpr const char* NITRATE_PRODUCTS = "nitrate products";
      static constexpr const char* X = "X";
      static constexpr const char* Y = "Y";
      static constexpr const char* A0 = "a0";
      static constexpr const char* N = "N";
      static constexpr const char* n = "n";

      static constexpr const char* PROBABILITY = "reaction probability";

      static constexpr const char* A = "A";
      static constexpr const char* B = "B";
      static constexpr const char* C = "C";
      static constexpr const char* D = "D";
      static constexpr const char* E = "E";
      static constexpr const char* Ea = "Ea";

      static constexpr const char* K0_A = "k0_A";
      static constexpr const char* K0_B = "k0_B";
      static constexpr const char* K0_C = "k0_C";
      static constexpr const char* KINF_A = "kinf_A";
      static constexpr const char* KINF_B = "kinf_B";
      static constexpr const char* KINF_C = "kinf_C";
      static constexpr const char* FC = "Fc";
    }  // namespace validation
  }  // namespace v0
}  // namespace mechanism_configuration
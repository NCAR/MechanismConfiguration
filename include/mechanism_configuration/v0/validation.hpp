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
      const struct Keys
      {
        const std::string NAME = "name";
        const std::string TYPE = "type";

        const std::string VALUE = "value";

        const std::string REACTIONS = "reactions";

        const std::string TRACER_TYPE = "tracer type";
        const std::string ABS_TOLERANCE = "absolute tolerance";
        const std::string DIFFUSION_COEFF = "diffusion coefficient [m2 s-1]";
        const std::string MOL_WEIGHT = "molecular weight [kg mol-1]";
        const std::string THIRD_BODY = "THIRD_BODY";

        const std::string REACTANTS = "reactants";
        const std::string PRODUCTS = "products";
        const std::string MUSICA_NAME = "MUSICA name";
        const std::string SCALING_FACTOR = "scaling factor";
        const std::string GAS_PHASE_REACTANT = "gas-phase reactant";
        const std::string GAS_PHASE_PRODUCTS = "gas-phase products";

        const std::string QTY = "qty";
        const std::string YIELD = "yield";

        const std::string SPECIES = "species";

        const std::string ALKOXY_PRODUCTS = "alkoxy products";
        const std::string NITRATE_PRODUCTS = "nitrate products";
        const std::string X = "X";
        const std::string Y = "Y";
        const std::string A0 = "a0";
        const std::string N = "N";
        const std::string n = "n";

        const std::string PROBABILITY = "reaction probability";

        const std::string A = "A";
        const std::string B = "B";
        const std::string C = "C";
        const std::string D = "D";
        const std::string E = "E";
        const std::string Ea = "Ea";

        const std::string K0_A = "k0_A";
        const std::string K0_B = "k0_B";
        const std::string K0_C = "k0_C";
        const std::string KINF_A = "kinf_A";
        const std::string KINF_B = "kinf_B";
        const std::string KINF_C = "kinf_C";
        const std::string FC = "Fc";
      } keys;
    }  // namespace validation
  }  // namespace v0
}  // namespace mechanism_configuration
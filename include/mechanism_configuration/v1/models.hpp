// Copyright (C) 2023-2025 University Corporation for Atmospheric Research, University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>

namespace mechanism_configuration
{
  namespace v1
  {
    namespace models
    {

      struct GaseModel
      {
        std::string name;
        std::string type;
        std::string phase;
      };

      struct Mode
      {
        std::string name;
        double geometric_mean_diameter;
        double geometric_standard_deviation;
        std::vector<std::string> phases;
      };

      struct ModalModel
      {
        std::string name;
        std::string type;
        std::vector<Mode> modes;
      };

      struct GasParticleInteraction
      {
        std::string name;
        GaseModel gas;
        ModalModel modal;
      };

    }
  }
}

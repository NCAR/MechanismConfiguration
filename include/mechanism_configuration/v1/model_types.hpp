// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace mechanism_configuration
{
  namespace v1
  {
    namespace types
    {

      struct GasModel
      {
        std::string name;
        std::string type;
        std::string phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct Mode
      {
        std::string name;
        double geometric_mean_diameter;
        double geometric_standard_deviation;
        std::string phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct ModalModel
      {
        std::string name;
        std::string type;
        std::vector<Mode> modes;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      // struct GasParticleInteraction
      // {
      //   std::string name;
      //   GasModel gas;
      //   ModalModel modal;
      // };

      /// @brief Represents a collection of different model types
      struct Models
      {
        GasModel gas_model;
        ModalModel modal_model;
      };
    }
  }
}

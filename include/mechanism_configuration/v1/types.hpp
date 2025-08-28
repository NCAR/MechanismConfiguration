// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <mechanism_configuration/errors.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    namespace types
    {

      struct Species
      {
        std::string name;
        std::optional<double> molecular_weight;
        std::optional<double> constant_concentration;
        std::optional<double> constant_mixing_ratio;
        std::optional<bool> is_third_body;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct PhaseSpecies
      {
        std::string name;
        std::optional<double> diffusion_coefficient;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct Phase
      {
        std::string name;
        std::vector<PhaseSpecies> species;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct ReactionComponent
      {
        std::string species_name;
        double coefficient{ 1.0 };
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

    }  // namespace types
  }  // namespace v1
}  // namespace mechanism_configuration
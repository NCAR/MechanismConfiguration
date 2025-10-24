// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace mechanism_configuration
{
  namespace development
  {
    namespace types
    {

      struct Species
      {
        std::string name;
        std::optional<double> absolute_tolerance;
        std::optional<double> diffusion_coefficient;
        std::optional<double> molecular_weight;
        std::optional<double> henrys_law_constant_298;
        std::optional<double> henrys_law_constant_exponential_factor;
        std::optional<double> n_star;
        std::optional<double> density;
        std::optional<std::string> tracer_type;
        std::optional<double> constant_concentration;
        std::optional<double> constant_mixing_ratio;
        std::optional<bool> is_third_body;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
        Errors validate();
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
        // TODO - Replace `species_name` with `name`.
        //        This will be removed once the decoupling is complete
        // std::string species_name;
        std::string name;
        double coefficient{ 1.0 };
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct Particle
      {
        /// @brief Describes the physical state of the particle
        std::string phase;
        /// @brief Lists the chemical species dissolved in the solvent
        std::vector<ReactionComponent> solutes;
        /// @brief Specifies the liquid medium in which solutes are dissolved
        std::vector<ReactionComponent> solvent;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

    }  // namespace types
  }  // namespace development
}  // namespace mechanism_configuration
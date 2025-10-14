// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/development/types.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration
{
  namespace development
  {
    /// @brief Validates a list of species definitions from a YAML node.
    ///        This function checks each species object in the input YAML list to ensure that:
    ///        - All required keys are present
    ///        - Optional keys are properly handled
    ///        - No duplicate species names exist
    /// @param species_list YAML node containing species entries
    /// @return List of validation errors, or empty if all entries are valid
    Errors ValidateSpecies(const YAML::Node& species_list);

    /// @brief Validates the structure and content of phase definitions in a YAML node.
    ///        Performs schema validation for each phase and its associated species.
    ///        - All required keys are present
    ///        - Detects duplicate species within a single phase
    ///        - Detects species not defined in the existing_species list
    ///        - Detects duplicate phase names
    /// @param phases_list YAML node containing the list of phase entries
    /// @param existing_species List of defined species to validate against
    /// @return List of validation errors, or empty if all entries are valid
    Errors ValidatePhases(
      const YAML::Node& phases_list, 
      const std::vector<types::Species>& existing_species);

          // TODO
    Errors ValidateReactantsOrProducts(const YAML::Node& object, const std::string& key);
    
    // TODO
    Errors ValidateReactions(
      const YAML::Node& reactions_list, 
      const std::vector<types::Species>& existing_species,
      const std::vector<types::Phase>& existing_phases
    );
  }  // namespace development
}  // namespace mechanism_configuration
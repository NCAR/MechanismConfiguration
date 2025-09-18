// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

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

  }  // namespace development
}  // namespace mechanism_configuration
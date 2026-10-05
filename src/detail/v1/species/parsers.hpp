// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/types/species.hpp>

#include <yaml-cpp/yaml.h>

#include <vector>

namespace mechanism_configuration::v1
{
  /// @brief Parses a YAML node into a vector of Species
  ///        The input must be validated using CheckSpeciesSchema().
  ///        This function assumes the structure and types are correct.
  /// @param objects YAML node representing species list
  /// @return A vector of parsed species
  std::vector<types::Species> ParseSpecies(const YAML::Node& objects);

  /// @brief Parses a YAML node into a vector of Phases
  ///        Extracts each phase's name and its associated species (including optional properties).
  ///        Assumes the input YAML has already been validated for required structure and keys.
  /// @param objects YAML node representing phase list
  /// @return A vector of parsed Phases
  std::vector<types::Phase> ParsePhases(const YAML::Node& objects);

}  // namespace mechanism_configuration::v1

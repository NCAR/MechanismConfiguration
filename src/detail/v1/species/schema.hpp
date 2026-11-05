// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

#include <string>

namespace mechanism_configuration::v1
{
  // Structural (schema) validation of v1 species and phases. These check required/optional keys
  // and value shape only; semantic invariants (duplicate names, unknown species, phase membership)
  // are checked separately by the version-neutral ValidateReactionsSemantics on the built Mechanism.

  /// @brief Schema-validates each species entry's keys.
  /// @param species_list YAML node containing species entries
  /// @param path JSON Pointer to the section. The errors of an item point to that item.
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckSpeciesSchema(const YAML::Node& species_list, const std::string& path);

  /// @brief Schema-validates each phase and its phase-species entries.
  /// @param phases_list YAML node containing the list of phase entries
  /// @param path JSON Pointer to the section. The errors of an item point to that item.
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckPhasesSchema(const YAML::Node& phases_list, const std::string& path);

}  // namespace mechanism_configuration::v1

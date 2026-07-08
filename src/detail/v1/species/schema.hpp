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
  // Structural (schema) validation of v1 species and phases. These check required/optional keys
  // and value shape only; semantic invariants (duplicate names, unknown species, phase membership)
  // are checked separately by the version-neutral ValidateSemantics on the built Mechanism.

  /// @brief Schema-validates each species entry's keys.
  /// @param species_list YAML node containing species entries
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckSpeciesSchema(const YAML::Node& species_list);

  /// @brief Schema-validates each phase and its phase-species entries.
  /// @param phases_list YAML node containing the list of phase entries
  /// @param existing_species Unused; retained for call-site compatibility
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckPhasesSchema(const YAML::Node& phases_list, const std::vector<types::Species>& existing_species);

}  // namespace mechanism_configuration::v1

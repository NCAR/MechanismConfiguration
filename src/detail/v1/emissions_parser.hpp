// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/types/emissions.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration::v1
{
  // Structural (schema) validation of the emissions section. Checks required/optional keys,
  // value shape, and fixed-enum-membership only; semantic invariants (duplicate names, unknown
  // inventory/species-map references, scaling-factor sums) are checked separately by the
  // version-neutral ValidateSemantics on the built Mechanism.

  /// @brief Schema-validates the emissions section: inventories, species maps (and their
  ///        mappings), regridding, and sources.
  /// @param emissions_node YAML node for the `emissions` key
  /// @return List of structural errors, or empty if the section conforms
  Errors CheckEmissionsSchema(const YAML::Node& emissions_node);

  /// @brief Parses a YAML node into an EmissionsConfig.
  ///        The input must be validated using CheckEmissionsSchema().
  ///        This function assumes the structure and types are correct.
  /// @param emissions_node YAML node for the `emissions` key
  /// @return The parsed EmissionsConfig
  types::EmissionsConfig ParseEmissionsSection(const YAML::Node& emissions_node);

}  // namespace mechanism_configuration::v1

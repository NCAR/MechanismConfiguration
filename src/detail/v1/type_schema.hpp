// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/types/species.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration::v1
{
  // Structural (schema) validation of v1 YAML. These check required/optional keys and value
  // shape only; semantic invariants (duplicate names, unknown species, phase membership) are
  // checked separately by the version-neutral ValidateSemantics on the built Mechanism.

  /// @brief Schema-validates each species entry's keys.
  /// @param species_list YAML node containing species entries
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckSpeciesSchema(const YAML::Node& species_list);

  /// @brief Schema-validates each phase and its phase-species entries.
  /// @param phases_list YAML node containing the list of phase entries
  /// @param existing_species Unused; retained for call-site compatibility
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckPhasesSchema(const YAML::Node& phases_list, const std::vector<types::Species>& existing_species);

  /// @brief Schema-validates a sequence of reaction components (reactants or products),
  ///        requiring exactly one of `name` / `species name` plus an optional coefficient.
  /// @param object YAML node representing a sequence of reactants or products
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckReactantsOrProductsSchema(const YAML::Node& object);

  /// @brief Schema-validates a YAML list of reactions: each has a defined, recognized type,
  ///        and then each reaction's keys are validated by its parser.
  /// @param reactions_list YAML node containing the list of reactions
  /// @param existing_species Unused; retained for call-site compatibility
  /// @param existing_phases Unused; retained for call-site compatibility
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckReactionsSchema(
      const YAML::Node& reactions_list,
      const std::vector<types::Species>& existing_species,
      const std::vector<types::Phase>& existing_phases);

}  // namespace mechanism_configuration::v1
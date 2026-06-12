// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/mechanism.hpp>

namespace mechanism_configuration
{
  /// @brief Validates the semantic invariants of a canonical Mechanism, independent of how it
  ///        was produced (parsed from any version, or constructed in code).
  ///
  /// Checks performed:
  ///  - species names are unique
  ///  - phase names are unique, each phase's species are unique and exist in the species list
  ///  - every species referenced by a reaction exists in the species list
  ///  - every reactant of a reaction is registered in that reaction's phase
  ///    (products may reference species from any phase)
  ///
  /// Structural/deserialization concerns (YAML keys, types, formatting) are the responsibility
  /// of the version-specific parsers and are not repeated here.
  ///
  /// @param mechanism The mechanism to validate.
  /// @return A collection of semantic errors; empty if the mechanism is valid.
  Errors validate(const Mechanism& mechanism);
}  // namespace mechanism_configuration

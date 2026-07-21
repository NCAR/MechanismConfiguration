// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/mechanism.hpp>

namespace mechanism_configuration
{

  /// @brief Validates a Mechanism's species, phases, and gas-phase reactions by
  ///        converting them to a location-free semantics::ReactionsInput and running
  ///        ValidateReactionsSemantics.
  Errors ValidateGasModel(const Mechanism& mechanism);

  /// @brief Validates aerosol cross-references against the mechanism's species and phases.
  ///        Checks phase/species references, representation-keyed rate-constant maps, and
  ///        required definition-derived properties.
  Errors ValidateAerosolModel(const Mechanism& mechanism);

  /// @brief Validates a Mechanism's emissions section by converting it to a location-free
  ///        semantics::EmissionsInput and running ValidateEmissionsSemantics. Returns no errors
  ///        if the mechanism has no emissions section.
  Errors ValidateEmissionsModel(const Mechanism& mechanism);

  /// @brief Validates the semantic invariants of a canonical Mechanism, regardless of whether it
  ///        was parsed or constructed in code. Combines ValidateGasModel, ValidateAerosolModel,
  ///        and ValidateEmissionsModel, returning all validation errors.
  Errors Validate(const Mechanism& mechanism);

}  // namespace mechanism_configuration

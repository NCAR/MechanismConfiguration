// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "detail/semantics/core.hpp"

#include <mechanism_configuration/errors.hpp>

#include <optional>
#include <string>
#include <vector>

namespace mechanism_configuration::semantics
{
  struct PhaseRef
  {
    std::string name;
    std::vector<NamedRef> species;
    std::optional<ErrorLocation> location;
  };

  struct ReactionRef
  {
    std::string type;
    std::string phase;
    std::vector<NamedRef> reactants;  // must exist and be registered in `phase`
    std::vector<NamedRef> products;   // must exist; may belong to any phase
    std::optional<ErrorLocation> location;
  };

  struct ReactionsInput
  {
    std::vector<NamedRef> species;
    std::vector<PhaseRef> phases;
    std::vector<ReactionRef> reactions;
  };
}  // namespace mechanism_configuration::semantics

namespace mechanism_configuration
{
  /// @brief Validates species, phase, gas-phase reaction semantic rules.
  ///        Errors include `line:col` when the source location is available.
  Errors ValidateReactionsSemantics(const semantics::ReactionsInput& input);

}  // namespace mechanism_configuration

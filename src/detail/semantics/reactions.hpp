// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <optional>
#include <string>
#include <vector>

namespace mechanism_configuration::semantics
{

  // Intermediate over which the core semantic validation (species, phases, gas-phase reactions)
  // runs. Both a parsed document and an in-code Mechanism lower into one of these and hand it to
  // ValidateReactionsSemantics, so those rules live in exactly one place. A parsed document fills
  // in source locations (so errors carry line:col); an in-code Mechanism leaves them empty.
  //
  // Emissions and aerosol cross-references each have their own separate engine (see
  // detail/semantics/emissions.hpp and detail/semantics/aerosol.hpp) — neither one ever
  // cross-references species/phases collected here, so bundling them into this Input would be
  // shared plumbing, not a shared rule set.
  //
  // This is an implementation detail of ValidateGasModel
  // (declared publicly in <mechanism_configuration/validate.hpp>) and is not part of the public API.
  struct NamedRef
  {
    std::string name;
    std::optional<ErrorLocation> location;
  };

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
  /// @brief The single home for the core (species / phase / gas-phase reaction) semantic rules.
  ///        Errors include `line:col` for any element whose source location was supplied.
  Errors ValidateReactionsSemantics(const semantics::ReactionsInput& input);

}  // namespace mechanism_configuration

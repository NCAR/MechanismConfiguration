// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "detail/semantics/core.hpp"

#include <optional>
#include <string>
#include <vector>

namespace mechanism_configuration::semantics
{
  // Definitions carry presence flags.
  struct SpeciesDef
  {
    std::string name;
    bool has_molecular_weight{ false };
  };

  struct PhaseSpeciesDef
  {
    std::string name;
    bool has_diffusion_coefficient{ false };
    bool has_density{ false };
  };

  struct PhaseDef
  {
    std::string name;
    std::vector<PhaseSpeciesDef> species;
  };

  struct AerosolRepresentationRef
  {
    std::string name;
    std::vector<NamedRef> phases;
    std::optional<ErrorLocation> location;
  };

  struct DissolvedReactionRef
  {
    NamedRef phase;
    NamedRef solvent;
    std::vector<NamedRef> reactants;
    std::vector<NamedRef> products;
    std::optional<ErrorLocation> location;
  };

  struct DissolvedReversibleReactionRef
  {
    NamedRef phase;
    NamedRef solvent;
    std::vector<NamedRef> reactants;
    std::vector<NamedRef> products;
    std::optional<ErrorLocation> location;
  };

  struct HenrysLawPhaseTransferRef
  {
    NamedRef gas_phase;
    NamedRef gas_species;
    NamedRef condensed_phase;
    NamedRef condensed_species;
    NamedRef solvent;
    std::optional<ErrorLocation> location;
  };

  struct HenrysLawEquilibriumRef
  {
    NamedRef gas_phase;
    NamedRef gas_species;
    NamedRef condensed_phase;
    NamedRef condensed_species;
    NamedRef solvent;
    std::optional<ErrorLocation> location;
  };

  struct DissolvedEquilibriumRef
  {
    NamedRef phase;
    NamedRef algebraic_species;
    NamedRef solvent;
    std::vector<NamedRef> reactants;
    std::vector<NamedRef> products;
    std::optional<ErrorLocation> location;
  };

  struct LinearConstraintTermRef
  {
    NamedRef phase;
    NamedRef species;
  };

  struct LinearConstraintRef
  {
    NamedRef algebraic_phase;
    NamedRef algebraic_species;
    std::vector<LinearConstraintTermRef> terms;
    std::optional<ErrorLocation> location;
  };

  struct AerosolInput
  {
    std::vector<SpeciesDef> species;
    std::vector<PhaseDef> phases;
    std::vector<AerosolRepresentationRef> representations;
    std::vector<DissolvedReactionRef> dissolved_reactions;
    std::vector<DissolvedReversibleReactionRef> dissolved_reversible_reactions;
    std::vector<HenrysLawPhaseTransferRef> henrys_law_phase_transfers;
    std::vector<HenrysLawEquilibriumRef> henrys_law_equilibria;
    std::vector<DissolvedEquilibriumRef> dissolved_equilibria;
    std::vector<LinearConstraintRef> linear_constraints;
  };

}  // namespace mechanism_configuration::semantics

namespace mechanism_configuration
{
  /// @brief Validates aerosol cross-references: phase/species membership (representations,
  ///        dissolved reactions/equilibria, Henry's-law transfers/equilibria, linear constraint
  ///        terms) and required definition-derived properties (molecular weight, diffusion
  ///        coefficient, density).
  Errors ValidateAerosolSemantics(const semantics::AerosolInput& input);
}  // namespace mechanism_configuration

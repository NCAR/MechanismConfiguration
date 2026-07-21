// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "detail/semantics/reactions.hpp"

#include <string>
#include <vector>

namespace mechanism_configuration::semantics
{
  // Not yet wired into ValidateAerosolModel (which still reads mechanism.aerosol directly).
  // Definitions carry presence flags instead of values: aerosol errors always report at the
  // reference site (e.g. "solvent 'foo' has no density"), never at the definition site, so
  // the definition's own location is never needed.

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
  };

  struct DissolvedReactionRef
  {
    NamedRef phase;
    NamedRef solvent;
    std::vector<NamedRef> reactants;
    std::vector<NamedRef> products;
  };

  struct DissolvedReversibleReactionRef
  {
    NamedRef phase;
    NamedRef solvent;
    std::vector<NamedRef> reactants;
    std::vector<NamedRef> products;
  };

  struct HenrysLawPhaseTransferRef
  {
    NamedRef gas_phase;
    NamedRef gas_species;
    NamedRef condensed_phase;
    NamedRef condensed_species;
    NamedRef solvent;
  };

  struct HenrysLawEquilibriumRef
  {
    NamedRef gas_phase;
    NamedRef gas_species;
    NamedRef condensed_phase;
    NamedRef condensed_species;
    NamedRef solvent;
  };

  struct DissolvedEquilibriumRef
  {
    NamedRef phase;
    NamedRef algebraic_species;
    NamedRef solvent;
    std::vector<NamedRef> reactants;
    std::vector<NamedRef> products;
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

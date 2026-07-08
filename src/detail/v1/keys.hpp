// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

// Keys shared across two or more v1 domains (species, reactions, aerosol, emissions).
// Domain-exclusive keys live in their own domain's keys.hpp.
namespace mechanism_configuration::v1::keys
{
  // Shared, but also Mechanism
  inline constexpr std::string_view version = "version";
  inline constexpr std::string_view name = "name";
  // Legacy alias for a reaction component's species reference, accepted alongside `name`
  // for backward compatibility with v1 configuration files.
  inline constexpr std::string_view species_name = "species name";

  // Configuration
  inline constexpr std::string_view species = "species";
  inline constexpr std::string_view phases = "phases";
  inline constexpr std::string_view reactions = "reactions";

  // Reactants and products (used by reactions and aerosol processes/constraints)
  inline constexpr std::string_view reactants = "reactants";
  inline constexpr std::string_view products = "products";
  inline constexpr std::string_view type = "type";
  inline constexpr std::string_view gas_phase = "gas phase";

  // Reactant and product
  inline constexpr std::string_view coefficient = "coefficient";
  // also name

  // Rate-constant parameters shared by ARRHENIUS-family reaction types and aerosol rate constants
  inline constexpr std::string_view A = "A";
  inline constexpr std::string_view B = "B";
  inline constexpr std::string_view C = "C";
  inline constexpr std::string_view D = "D";
  inline constexpr std::string_view E = "E";
  inline constexpr std::string_view Ea = "Ea";

  // Phase-transfer fields shared by the SURFACE reaction and aerosol phase transfer/equilibrium
  inline constexpr std::string_view gas_phase_species = "gas-phase species";
  inline constexpr std::string_view condensed_phase = "condensed phase";

  // Scaling factor shared across several reaction types and emissions species maps
  inline constexpr std::string_view scaling_factor = "scaling factor";

}  // namespace mechanism_configuration::v1::keys

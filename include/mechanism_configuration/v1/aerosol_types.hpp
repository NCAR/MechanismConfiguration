// Copyright (C) 2026 University Corporation for Atmospheric Research
//                     University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/v1/types.hpp>

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace mechanism_configuration
{
  namespace v1
  {
    namespace types
    {

      // ── Rate constant types used by aerosol processes ─────────────────

      /// @brief Arrhenius rate constant: k(T) = A * exp(-C * (1/T - 1/298.15))
      struct AerosolArrheniusRate
      {
        double A{ 1.0 };
        double C{ 0.0 };
      };

      /// @brief Equilibrium constant: K(T) = A * exp(C * (1/298.15 - 1/T))
      struct AerosolEquilibriumConstant
      {
        double A{ 1.0 };
        double C{ 0.0 };
      };

      /// @brief Henry's law constant: HLC(T) = HLC_ref * exp(C * (1/T - 1/298.15))
      struct HenrysLawConstant
      {
        double hlc_ref;  ///< Reference HLC at 298.15 K [mol m-3 Pa-1]
        double C{ 0.0 }; ///< Temperature dependence factor [K]
      };

      // ── Aerosol Representations ───────────────────────────────────────

      struct UniformSection
      {
        std::string name;
        std::vector<std::string> phases;
        double min_radius;  ///< [m]
        double max_radius;  ///< [m]
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct SingleMomentMode
      {
        std::string name;
        std::vector<std::string> phases;
        double geometric_mean_radius;         ///< [m]
        double geometric_standard_deviation;  ///< [-]
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct TwoMomentMode
      {
        std::string name;
        std::vector<std::string> phases;
        double geometric_standard_deviation;  ///< [-]
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      using AerosolRepresentation = std::variant<UniformSection, SingleMomentMode, TwoMomentMode>;

      // ── Aerosol Processes ─────────────────────────────────────────────

      struct HenryLawEquilibrium
      {
        std::string gas_phase;
        std::string gas_phase_species;
        std::string condensed_phase;
        std::string condensed_phase_species;
        std::string solvent;
        HenrysLawConstant henrys_law_constant;
        std::string name;
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct HenryLawPhaseTransfer
      {
        std::string gas_phase;
        std::string gas_phase_species;
        std::string condensed_phase;
        std::string condensed_phase_species;
        std::string solvent;
        HenrysLawConstant henrys_law_constant;
        double diffusion_coefficient;      ///< [m2 s-1]
        double accommodation_coefficient;  ///< [-]
        std::string name;
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct DissolvedReaction
      {
        std::string condensed_phase;
        std::string solvent;
        std::vector<ReactionComponent> reactants;
        std::vector<ReactionComponent> products;
        AerosolArrheniusRate rate_constant;
        double min_halflife = 0.0;  ///< When > 0, caps rate so no reactant depleted faster than this half-life [s]
        std::string name;
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct DissolvedReversibleReaction
      {
        std::string condensed_phase;
        std::string solvent;
        std::vector<ReactionComponent> reactants;
        std::vector<ReactionComponent> products;
        std::optional<AerosolArrheniusRate> forward_rate_constant;
        std::optional<AerosolArrheniusRate> reverse_rate_constant;
        std::optional<AerosolEquilibriumConstant> equilibrium_constant;
        std::string name;
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct DissolvedEquilibrium
      {
        std::string condensed_phase;
        std::string solvent;
        std::vector<ReactionComponent> reactants;
        std::vector<ReactionComponent> products;
        std::string algebraic_species;
        AerosolEquilibriumConstant equilibrium_constant;
        std::string name;
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct LinearConstraintTerm
      {
        std::string phase;
        std::string species;
        double coefficient{ 1.0 };
      };

      struct LinearConstraint
      {
        std::string name;
        std::string algebraic_phase;
        std::string algebraic_species;
        std::vector<LinearConstraintTerm> terms;
        double constant{ 0.0 };
        bool diagnose_from_state{ false };
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      using AerosolProcess = std::variant<
          HenryLawEquilibrium,
          HenryLawPhaseTransfer,
          DissolvedReaction,
          DissolvedReversibleReaction,
          DissolvedEquilibrium,
          LinearConstraint>;

    }  // namespace types
  }  // namespace v1
}  // namespace mechanism_configuration

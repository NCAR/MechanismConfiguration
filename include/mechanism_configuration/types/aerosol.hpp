// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/types/reactions.hpp>

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace mechanism_configuration::types
{
  // ----------------------------------------
  // Rate constants
  // ----------------------------------------

  /// @brief Reference-temperature Arrhenius
  ///        f(T) = A * exp( C * (1/T0 - 1/T) )      (C = +Ea/R, positive)
  struct Equilibrium
  {
    double A;            ///< Value at the reference temperature T0 [units vary by use]
    double C = 0.0;      ///< Temperature-dependence parameter [K] (C = +Ea/R)
    double T0 = 298.15;  ///< Reference temperature [K]
  };

  /// @brief Henry's law constant: HLC(T) = HLC_ref * exp( C * (1/T - 1/T0) )
  ///        Same as Equilibrium but with the
  ///        opposite temperature trend (solubility rises as T falls)
  struct HenryLawConstant
  {
    double HLC_ref;      ///< Reference HLC at T0 [mol m-3 Pa-1]
    double C = 0.0;      ///< Temperature-dependence parameter [K]
    double T0 = 298.15;  ///< Reference temperature [K]
  };

  /// @brief A reaction rate constant parsed from config.
  using RateConstant = std::variant<Arrhenius, Equilibrium, std::function<double(double)>>;

  // ----------------------------------------
  // Representations
  // ----------------------------------------

  struct UniformSection
  {
    std::string name;
    std::vector<std::string> phases;
    double min_radius;  ///< [m]
    double max_radius;  ///< [m]
  };

  struct SingleMomentMode
  {
    std::string name;
    std::vector<std::string> phases;
    double geometric_mean_radius;         ///< [m]
    double geometric_standard_deviation;  ///< dimensionless
  };

  struct TwoMomentMode
  {
    std::string name;
    std::vector<std::string> phases;
    double geometric_standard_deviation;  ///< dimensionless
  };

  using Representation = std::variant<UniformSection, SingleMomentMode, TwoMomentMode>;

  // ----------------------------------------
  // Processes
  // ----------------------------------------

  struct DissolvedReaction
  {
    std::string phase;
    std::string solvent;
    std::vector<ReactionComponent> reactants;
    std::vector<ReactionComponent> products;
    /// @brief Rate constant per aerosol representation; keys are representation names.
    std::map<std::string, RateConstant> rate_constants;
    std::optional<double> solvent_floor_;
    std::optional<double> min_halflife_;
  };

  struct DissolvedReversibleReaction
  {
    std::string phase;
    std::string solvent;
    std::vector<ReactionComponent> reactants;
    std::vector<ReactionComponent> products;
    /// @brief Per-representation forward / reverse rate constants; keys are representation names.
    ///        Supply exactly two of {forward, reverse, equilibrium} per representation; the third is derived.
    std::map<std::string, RateConstant> forward_rate_constants;
    std::map<std::string, RateConstant> reverse_rate_constants;
    /// @brief Shared, intrinsic equilibrium constant (NOT per representation).
    std::optional<Equilibrium> equilibrium_constant;
    std::optional<double> solvent_floor_;
  };

  struct HenryLawPhaseTransfer
  {
    std::string gas_phase;
    std::string gas_species;
    std::string condensed_phase;
    std::string condensed_species;
    std::string solvent;
    HenryLawConstant henry_law_constant;
    double diffusion_coefficient;      ///< Gas-phase diffusion coefficient [m2 s-1]
    double accommodation_coefficient;  ///< Mass accommodation coefficient, dimensionless
  };

  using Process = std::variant<DissolvedReaction, DissolvedReversibleReaction, HenryLawPhaseTransfer>;

  // ----------------------------------------
  // Constraints
  // ----------------------------------------

  struct HenryLawEquilibrium
  {
    std::string gas_phase;
    std::string gas_species;
    std::string condensed_phase;
    std::string condensed_species;
    std::string solvent;
    HenryLawConstant henry_law_constant;
    double solvent_molecular_weight;  ///< [kg mol-1]
    double solvent_density;           ///< [kg m-3]
  };

  struct DissolvedEquilibrium
  {
    std::string phase;
    std::string algebraic_species;
    std::string solvent;
    std::vector<ReactionComponent> reactants;
    std::vector<ReactionComponent> products;
    Equilibrium equilibrium_constant;
    std::optional<double> solvent_floor_;
  };

  struct LinearConstraintTerm
  {
    std::string phase;
    std::string name;
    double coefficient;
  };

  /// @brief RHS constant C of a LinearConstraint:  G = sum(coeff_i * [species_i]) - C = 0
  ///        Choose exactly one mode (mutually exclusive):
  struct FixedConstant
  {
    double value = 0.0;
  };  ///< Fixed value [mol m-3], shared by all instances
  struct DiagnoseFromState
  {
  };  ///< Computed per representation instance

  struct LinearConstraint
  {
    std::string algebraic_phase;
    std::string algebraic_species;
    std::vector<LinearConstraintTerm> terms;
    std::variant<FixedConstant, DiagnoseFromState> constant = FixedConstant{ 0.0 };
  };

  using Constraint = std::variant<HenryLawEquilibrium, DissolvedEquilibrium, LinearConstraint>;

  // ----------------------------------------
  // Container
  // ----------------------------------------

  /// @brief Collection of parsed aerosol entries
  struct Aerosol
  {
    std::vector<Representation> representations;
    std::vector<Process> processes;
    std::vector<Constraint> constraints;
  };

}  // namespace mechanism_configuration::types
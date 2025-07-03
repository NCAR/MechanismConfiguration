// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/v1/types.hpp>

#include <array>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace mechanism_configuration
{
  namespace v1
  {
    namespace types
    {

      struct Arrhenius
      {
        /// @brief Pre-exponential factor [(mol m−3)^(−(𝑛−1)) s−1]
        double A{ 1 };
        /// @brief Unitless exponential factor
        double B{ 0 };
        /// @brief Activation threshold, expected to be the negative activation energy divided by the boltzman constant
        ///        [-E_a / k_b), K]
        double C{ 0 };
        /// @brief A factor that determines temperature dependence [K]
        double D{ 300 };
        /// @brief A factor that determines pressure dependence [Pa-1]
        double E{ 0 };
        /// @brief A list of reactants
        std::vector<ReactionComponent> reactants;
        /// @brief A list of products
        std::vector<ReactionComponent> products;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier indicating which gas phase this reaction takes place in
        std::string gas_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct Branched
      {
        /// @brief pre-exponential factor
        double X;
        /// @brief exponential factor
        double Y;
        /// @brief branching factor
        double a0;
        /// @brief number of heavy atoms in the RO2 reacting species (excluding the peroxy moiety)
        int n;
        /// @brief A list of reactants
        std::vector<ReactionComponent> reactants;
        /// @brief A list of nitrate products
        std::vector<ReactionComponent> nitrate_products;
        /// @brief A list of alkoxy products
        std::vector<ReactionComponent> alkoxy_products;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier indicating which gas phase this reaction takes place in
        std::string gas_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct CondensedPhaseArrhenius
      {
        /// @brief Pre-exponential factor [(mol m−3)^(−(𝑛−1)) s−1]
        double A{ 1 };
        /// @brief Unitless exponential factor
        double B{ 0 };
        /// @brief Activation threshold, expected to be the negative activation energy divided by the boltzman constant
        ///        [-E_a / k_b), K]
        double C{ 0 };
        /// @brief A factor that determines temperature dependence [K]
        double D{ 300 };
        /// @brief A factor that determines pressure dependence [Pa-1]
        double E{ 0 };
        /// @brief A list of reactants
        std::vector<ReactionComponent> reactants;
        /// @brief A list of products
        std::vector<ReactionComponent> products;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier for the aqueous phase where this reaction occurs
        std::string aqueous_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct CondensedPhasePhotolysis
      {
        /// @brief Scaling factor to apply to user-provided rate constants
        double scaling_factor{ 1.0 };
        /// @brief A list of reactants
        std::vector<ReactionComponent> reactants;
        /// @brief A list of products
        std::vector<ReactionComponent> products;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier for the aqueous phase where this reaction occurs
        std::string aqueous_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct Emission
      {
        /// @brief Scaling factor to apply to user-provided rate constants
        double scaling_factor{ 1.0 };
        /// @brief A list of products
        std::vector<ReactionComponent> products;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier indicating which gas phase this reaction takes place in
        std::string gas_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct FirstOrderLoss
      {
        /// @brief Scaling factor to apply to user-provided rate constants
        double scaling_factor{ 1.0 };
        /// @brief A list of reactants
        std::vector<ReactionComponent> reactants;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier indicating which gas phase this reaction takes place in
        std::string gas_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct SimpolPhaseTransfer
      {
        /// @brief An identifier indicating which gas phase this reaction takes place in
        std::string gas_phase;
        /// @brief The species in the gas phase participating in this reaction
        ReactionComponent gas_phase_species;
        /// @brief An identifier for the aqueous phase where this reaction occurs
        std::string aqueous_phase;
        /// @brief The species in the aqueous phase participating in this reaction
        ReactionComponent aqueous_phase_species;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief The 4 SIMPOL parameters
        std::array<double, 4> B;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct AqueousEquilibrium
      {
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier indicating which gas phase this reaction takes place in
        std::string gas_phase;
        /// @brief An identifier for the aqueous phase where this reaction occurs
        std::string aqueous_phase;
        /// @brief A list of reactants
        std::vector<ReactionComponent> reactants;
        /// @brief A list of products
        std::vector<ReactionComponent> products;
        /// @brief Pre-exponential factor (s-1)
        double A{ 1 };
        /// @brief A constant
        double C{ 0 };
        /// @brief Reverse reation rate constant (s-1)
        double k_reverse{ 0 };
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct WetDeposition
      {
        /// @brief Scaling factor to apply to user-provided rate constants
        double scaling_factor{ 1.0 };
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier for the aqueous phase where this reaction occurs
        std::string aqueous_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct HenrysLaw
      {
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief Represents the composition of a gas mixture
        Phase gas;
        /// @brief Represents a particle within a solution, including its phase and chemical composition
        Particle particle;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct Photolysis
      {
        /// @brief Scaling factor to apply to user-provided rate constants
        double scaling_factor{ 1.0 };
        /// @brief A list of reactants
        std::vector<ReactionComponent> reactants;
        /// @brief A list of products
        std::vector<ReactionComponent> products;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier indicating which gas phase this reaction takes place in
        std::string gas_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct Surface
      {
        /// @brief Reaction probability (0-1) [unitless]
        double reaction_probability{ 1.0 };
        /// @brief A list of reactants
        ReactionComponent gas_phase_species;
        /// @brief A list of products
        std::vector<ReactionComponent> gas_phase_products;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier indicating which gas phase this reaction takes place in
        std::string gas_phase;
        /// @brief An identifier for the aqueous phase where this reaction occurs
        std::string aqueous_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct TaylorSeries
      {
        /// @brief Pre-exponential factor [(mol m−3)^(−(𝑛−1)) s−1]
        double A{ 1 };
        /// @brief Unitless exponential factor
        double B{ 0 };
        /// @brief Activation threshold, expected to be the negative activation energy divided by the boltzman constant
        ///        [-E_a / k_b), K]
        double C{ 0 };
        /// @brief A factor that determines temperature dependence [K]
        double D{ 300 };
        /// @brief A factor that determines pressure dependence [Pa-1]
        double E{ 0 };
        /// @brief An array of coefficients for the Taylor series expansion
        std::vector<double> taylor_coefficients{ 1.0 };
        /// @brief A list of reactants
        std::vector<ReactionComponent> reactants;
        /// @brief A list of products
        std::vector<ReactionComponent> products;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier indicating which gas phase this reaction takes place in
        std::string gas_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct Troe
      {
        /// @brief low-pressure pre-exponential factor
        double k0_A = 1.0;
        /// @brief low-pressure temperature-scaling parameter
        double k0_B = 0.0;
        /// @brief low-pressure exponential factor
        double k0_C = 0.0;
        /// @brief high-pressure pre-exponential factor
        double kinf_A = 1.0;
        /// @brief high-pressure temperature-scaling parameter
        double kinf_B = 0.0;
        /// @brief high-pressure exponential factor
        double kinf_C = 0.0;
        /// @brief Troe F_c parameter
        double Fc = 0.6;
        /// @brief Troe N parameter
        double N = 1.0;
        /// @brief A list of reactants
        std::vector<ReactionComponent> reactants;
        /// @brief A list of products
        std::vector<ReactionComponent> products;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier indicating which gas phase this reaction takes place in
        std::string gas_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct Tunneling
      {
        /// @brief Pre-exponential factor [(mol m−3)^(−(𝑛−1)) s−1]
        double A = 1.0;
        /// @brief Linear temperature-dependent parameter [K]
        double B = 0.0;
        /// @brief Cubed temperature-dependent parameter [K^3]
        double C = 0.0;
        /// @brief A list of reactants
        std::vector<ReactionComponent> reactants;
        /// @brief A list of products
        std::vector<ReactionComponent> products;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier indicating which gas phase this reaction takes place in
        std::string gas_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      struct UserDefined
      {
        /// @brief Scaling factor to apply to user-provided rate constants
        double scaling_factor{ 1.0 };
        /// @brief A list of reactants
        std::vector<ReactionComponent> reactants;
        /// @brief A list of products
        std::vector<ReactionComponent> products;
        /// @brief An identifier, optional, uniqueness not enforced
        std::string name;
        /// @brief An identifier indicating which gas phase this reaction takes place in
        std::string gas_phase;
        /// @brief Unknown properties, prefixed with two underscores (__)
        std::unordered_map<std::string, std::string> unknown_properties;
      };

      /// @brief Represents a collection of different reaction types
      struct Reactions
      {
        std::vector<Arrhenius> arrhenius;
        std::vector<Branched> branched;
        std::vector<CondensedPhaseArrhenius> condensed_phase_arrhenius;
        std::vector<CondensedPhasePhotolysis> condensed_phase_photolysis;
        std::vector<Emission> emission;
        std::vector<FirstOrderLoss> first_order_loss;
        std::vector<SimpolPhaseTransfer> simpol_phase_transfer;
        std::vector<AqueousEquilibrium> aqueous_equilibrium;
        std::vector<WetDeposition> wet_deposition;
        std::vector<HenrysLaw> henrys_law;
        std::vector<Photolysis> photolysis;
        std::vector<Surface> surface;
        std::vector<TaylorSeries> taylor_series;
        std::vector<Troe> troe;
        std::vector<Tunneling> tunneling;
        std::vector<UserDefined> user_defined;
      };

    }  // namespace types
  }  // namespace v1
}  // namespace mechanism_configuration
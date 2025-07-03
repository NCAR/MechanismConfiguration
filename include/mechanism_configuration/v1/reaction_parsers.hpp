// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/v1/reaction_types.hpp>
#include <mechanism_configuration/v1/types.hpp>

#include <yaml-cpp/yaml.h>

#include <vector>

namespace mechanism_configuration
{
  namespace v1
  {

    /// @brief Abstract interface for reaction parsers
    class IReactionParser
    {
     public:
      /// @brief Parses a YAML node representing a chemical reaction
      /// @param object The YAML node containing reaction information
      /// @param existing_species A list of species previously defined in the mechanism
      /// @param existing_phases A list of chemical phases relevant to the reaction
      /// @param reactions The container to which the parsed reactions will be added
      /// @return A list of any parsing errors encountered
      virtual Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) = 0;

      /// @brief Destructor
      virtual ~IReactionParser() = default;
    };

    class ArrheniusParser : public IReactionParser
    {
     public:
      /// @brief Parser for Arrhenius reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class BranchedParser : public IReactionParser
    {
     public:
      /// @brief Parser for branched reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class CondensedPhaseArrheniusParser : public IReactionParser
    {
     public:
      /// @brief Parser for condensed-phase Arrhenius reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class CondensedPhasePhotolysisParser : public IReactionParser
    {
     public:
      /// @brief Parser for condensed-phase photolysis reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class EmissionParser : public IReactionParser
    {
     public:
      /// @brief Parser for emission reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class FirstOrderLossParser : public IReactionParser
    {
     public:
      /// @brief Parser for first-order loss reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class SimpolPhaseTransferParser : public IReactionParser
    {
     public:
      /// @brief Parser for SIMPOL-phase transfer reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class AqueousEquilibriumParser : public IReactionParser
    {
     public:
      /// @brief Parser for aqueous equilibrium reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class WetDepositionParser : public IReactionParser
    {
     public:
      /// @brief Parser for wet deposition reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class HenrysLawParser : public IReactionParser
    {
     public:
      /// @brief Parser for Henry's Law reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class PhotolysisParser : public IReactionParser
    {
     public:
      /// @brief Parser for photolysis reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class SurfaceParser : public IReactionParser
    {
     public:
      /// @brief Parser for surface reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class TaylorSeriesParser : public IReactionParser
    {
     public:
      /// @brief Parser for Taylor series reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class TroeParser : public IReactionParser
    {
     public:
      /// @brief Parser for Troe reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class TunnelingParser : public IReactionParser
    {
     public:
      /// @brief Parser for tunneling reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class UserDefinedParser : public IReactionParser
    {
     public:
      /// @brief Parser for user-defined reactions
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

  }  // namespace v1
}  // namespace mechanism_configuration

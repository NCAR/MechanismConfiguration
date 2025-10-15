// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/types.hpp>
#include <mechanism_configuration/development/validation.hpp>
#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

#include <vector>

namespace mechanism_configuration
{
  namespace development
  {

    /// @brief Abstract interface for reaction parsers
    class IReactionParser
    {
     public:

      virtual Errors Validate(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
      {
        return Errors();
      }

      virtual void Parse(const YAML::Node& object, types::Reactions& reactions)
      {
        // Optional during the development
      }

      // TODO(in progress) - Will remove this function when the issue 148 is complete
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
          types::Reactions& reactions)
      {
        return Errors();
      }

      /// @brief Destructor
      virtual ~IReactionParser() = default;
    };

    class ArrheniusParser : public IReactionParser
    {
     public:

      Errors Validate(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Reactions& reactions) override;
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

    class TernaryChemicalActivationParser : public IReactionParser
    {
     public:
      /// @brief Parser for Ternary Chemical Activation reactions
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

    /// @brief Returns a static map of reaction type keys to their parser instances
    inline std::map<std::string, std::unique_ptr<IReactionParser>>& GetReactionParserMap()
    {
      static std::map<std::string, std::unique_ptr<IReactionParser>> reaction_parsers = [] {
        std::map<std::string, std::unique_ptr<IReactionParser>> map;
        map[validation::Arrhenius_key] = std::make_unique<ArrheniusParser>();
        map[validation::HenrysLaw_key] = std::make_unique<HenrysLawParser>();
        map[validation::WetDeposition_key] = std::make_unique<WetDepositionParser>();
        map[validation::AqueousPhaseEquilibrium_key] = std::make_unique<AqueousEquilibriumParser>();
        map[validation::SimpolPhaseTransfer_key] = std::make_unique<SimpolPhaseTransferParser>();
        map[validation::FirstOrderLoss_key] = std::make_unique<FirstOrderLossParser>();
        map[validation::Emission_key] = std::make_unique<EmissionParser>();
        map[validation::CondensedPhasePhotolysis_key] = std::make_unique<CondensedPhasePhotolysisParser>();
        map[validation::Photolysis_key] = std::make_unique<PhotolysisParser>();
        map[validation::Surface_key] = std::make_unique<SurfaceParser>();
        map[validation::TaylorSeries_key] = std::make_unique<TaylorSeriesParser>();
        map[validation::Tunneling_key] = std::make_unique<TunnelingParser>();
        map[validation::Branched_key] = std::make_unique<BranchedParser>();
        map[validation::Troe_key] = std::make_unique<TroeParser>();
        map[validation::TernaryChemicalActivation_key] = std::make_unique<TernaryChemicalActivationParser>();
        map[validation::CondensedPhaseArrhenius_key] = std::make_unique<CondensedPhaseArrheniusParser>();
        map[validation::UserDefined_key] = std::make_unique<UserDefinedParser>();
        return map;
      }();
      
      return reaction_parsers;
    }

  }  // namespace development
}  // namespace mechanism_configuration

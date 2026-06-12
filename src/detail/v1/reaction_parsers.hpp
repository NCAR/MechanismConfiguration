// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/types.hpp>
#include <detail/validation_keys.hpp>
#include <mechanism_configuration/errors.hpp>

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
      /// @brief Validates a YAML node representing a chemical reaction
      /// @param object The YAML node containing reaction information
      /// @param existing_species A list of species previously defined in the mechanism
      /// @param existing_phases A list of chemical phases relevant to the reaction
      /// @return A list of any validation errors encountered
      virtual Errors Validate(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases) = 0;

      /// @brief Parses a YAML node representing a chemical reaction
      /// @param object The YAML node containing reaction information
      /// @param reactions The container to which the parsed reactions will be added
      virtual void Parse(const YAML::Node& object, types::Reactions& reactions) = 0;

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
      Errors Validate(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Reactions& reactions) override;
    };

    class EmissionParser : public IReactionParser
    {
     public:
      Errors Validate(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Reactions& reactions) override;
    };

    class FirstOrderLossParser : public IReactionParser
    {
     public:
      Errors Validate(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Reactions& reactions) override;
    };

    class PhotolysisParser : public IReactionParser
    {
     public:
      Errors Validate(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Reactions& reactions) override;
    };

    class SurfaceParser : public IReactionParser
    {
     public:
      Errors Validate(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Reactions& reactions) override;
    };

    class TaylorSeriesParser : public IReactionParser
    {
     public:
      Errors Validate(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Reactions& reactions) override;
    };

    class TroeParser : public IReactionParser
    {
     public:
      Errors Validate(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Reactions& reactions) override;
    };

    class TernaryChemicalActivationParser : public IReactionParser
    {
     public:
      Errors Validate(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Reactions& reactions) override;
    };

    class TunnelingParser : public IReactionParser
    {
     public:
      Errors Validate(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Reactions& reactions) override;
    };

    class UserDefinedParser : public IReactionParser
    {
     public:
      Errors Validate(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Reactions& reactions) override;
    };

    class LambdaRateConstantParser : public IReactionParser
    {
     public:
      Errors Validate(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Reactions& reactions) override;
    };

    /// @brief Returns a static map of reaction type keys to their parser instances
    inline std::map<std::string, std::unique_ptr<IReactionParser>>& GetReactionParserMap()
    {
      static std::map<std::string, std::unique_ptr<IReactionParser>> reaction_parsers = []
      {
        std::map<std::string, std::unique_ptr<IReactionParser>> map;
        map[std::string(validation::Arrhenius_key)] = std::make_unique<ArrheniusParser>();
        map[std::string(validation::FirstOrderLoss_key)] = std::make_unique<FirstOrderLossParser>();
        map[std::string(validation::Emission_key)] = std::make_unique<EmissionParser>();
        map[std::string(validation::Photolysis_key)] = std::make_unique<PhotolysisParser>();
        map[std::string(validation::Surface_key)] = std::make_unique<SurfaceParser>();
        map[std::string(validation::TaylorSeries_key)] = std::make_unique<TaylorSeriesParser>();
        map[std::string(validation::Tunneling_key)] = std::make_unique<TunnelingParser>();
        map[std::string(validation::Branched_key)] = std::make_unique<BranchedParser>();
        map[std::string(validation::Troe_key)] = std::make_unique<TroeParser>();
        map[std::string(validation::TernaryChemicalActivation_key)] = std::make_unique<TernaryChemicalActivationParser>();
        map[std::string(validation::UserDefined_key)] = std::make_unique<UserDefinedParser>();
        map[std::string(validation::LambdaRateConstant_key)] = std::make_unique<LambdaRateConstantParser>();
        return map;
      }();

      return reaction_parsers;
    }

  }  // namespace v1
}  // namespace mechanism_configuration

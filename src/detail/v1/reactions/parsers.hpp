// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/types/reactions.hpp>
#include <mechanism_configuration/types/species.hpp>

#include <detail/v1/reactions/keys.hpp>
#include <yaml-cpp/yaml.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace mechanism_configuration::v1
{
  /// @brief Parses a YAML node into reaction components
  /// @param object YAML node representing ReactionComponents
  /// @param key Key of the sequence to parse
  /// @return Vector of `types::ReactionComponent` with names, optional coefficients, and comments
  std::vector<types::ReactionComponent> ParseReactionComponents(const YAML::Node& object, std::string_view key);

  /// @brief Parses a single reaction component from a YAML node.
  ///        The parser performs no validation or error checking.
  /// @param object YAML node representing ReactionComponents
  /// @param key Key identifying the reaction component
  /// @return The parsed `types::ReactionComponent`, or a default-constructed one if none found
  types::ReactionComponent ParseReactionComponent(const YAML::Node& object, std::string_view key);

  /// @brief Parses a collection of YAML nodes into reaction objects
  ///        Iterates over the given YAML nodes, identifies the parser for each reaction type,
  ///        and populates a `types::Reactions` container with the parsed reactions.
  /// @param objects YAML node containing multiple reaction definitions
  /// @return A `types::Reactions` object with all successfully parsed reactions
  types::Reactions ParseReactions(const YAML::Node& objects);

  /// @brief Abstract interface for reaction parsers
  class IReactionParser
  {
   public:
    /// @brief Checks the schema of a YAML node representing a chemical reaction
    /// @param object The YAML node containing reaction information
    /// @param existing_species A list of species previously defined in the mechanism
    /// @param existing_phases A list of chemical phases relevant to the reaction
    /// @return A list of any validation errors encountered
    virtual Errors CheckSchema(
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
    Errors CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases) override;

    void Parse(const YAML::Node& object, types::Reactions& reactions) override;
  };

  class BranchedParser : public IReactionParser
  {
   public:
    Errors CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases) override;

    void Parse(const YAML::Node& object, types::Reactions& reactions) override;
  };

  class EmissionParser : public IReactionParser
  {
   public:
    Errors CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases) override;

    void Parse(const YAML::Node& object, types::Reactions& reactions) override;
  };

  class FirstOrderLossParser : public IReactionParser
  {
   public:
    Errors CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases) override;

    void Parse(const YAML::Node& object, types::Reactions& reactions) override;
  };

  class PhotolysisParser : public IReactionParser
  {
   public:
    Errors CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases) override;

    void Parse(const YAML::Node& object, types::Reactions& reactions) override;
  };

  class SurfaceParser : public IReactionParser
  {
   public:
    Errors CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases) override;

    void Parse(const YAML::Node& object, types::Reactions& reactions) override;
  };

  class TaylorSeriesParser : public IReactionParser
  {
   public:
    Errors CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases) override;

    void Parse(const YAML::Node& object, types::Reactions& reactions) override;
  };

  class TroeParser : public IReactionParser
  {
   public:
    Errors CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases) override;

    void Parse(const YAML::Node& object, types::Reactions& reactions) override;
  };

  class TernaryChemicalActivationParser : public IReactionParser
  {
   public:
    Errors CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases) override;

    void Parse(const YAML::Node& object, types::Reactions& reactions) override;
  };

  class TunnelingParser : public IReactionParser
  {
   public:
    Errors CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases) override;

    void Parse(const YAML::Node& object, types::Reactions& reactions) override;
  };

  class UserDefinedParser : public IReactionParser
  {
   public:
    Errors CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases) override;

    void Parse(const YAML::Node& object, types::Reactions& reactions) override;
  };

  class LambdaRateConstantParser : public IReactionParser
  {
   public:
    Errors CheckSchema(
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
      map[std::string(keys::Arrhenius_key)] = std::make_unique<ArrheniusParser>();
      map[std::string(keys::FirstOrderLoss_key)] = std::make_unique<FirstOrderLossParser>();
      map[std::string(keys::Emission_key)] = std::make_unique<EmissionParser>();
      map[std::string(keys::Photolysis_key)] = std::make_unique<PhotolysisParser>();
      map[std::string(keys::Surface_key)] = std::make_unique<SurfaceParser>();
      map[std::string(keys::TaylorSeries_key)] = std::make_unique<TaylorSeriesParser>();
      map[std::string(keys::Tunneling_key)] = std::make_unique<TunnelingParser>();
      map[std::string(keys::Branched_key)] = std::make_unique<BranchedParser>();
      map[std::string(keys::Troe_key)] = std::make_unique<TroeParser>();
      map[std::string(keys::TernaryChemicalActivation_key)] = std::make_unique<TernaryChemicalActivationParser>();
      map[std::string(keys::UserDefined_key)] = std::make_unique<UserDefinedParser>();
      map[std::string(keys::LambdaRateConstant_key)] = std::make_unique<LambdaRateConstantParser>();
      return map;
    }();

    return reaction_parsers;
  }
}  // namespace mechanism_configuration::v1

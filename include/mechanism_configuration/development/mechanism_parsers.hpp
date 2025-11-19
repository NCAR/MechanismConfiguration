// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/development/model_types.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/types.hpp>
#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

#include <utility>
#include <vector>

namespace mechanism_configuration
{
  namespace development
  {
    /// @brief Parses a YAML node into a vector of Species
    ///        The input must be validated using ValidateSpecies().
    ///        This function assumes the structure and types are correct.
    /// @param objects YAML node representing species list
    /// @return A vector of parsed species
    std::vector<types::Species> ParseSpecies(const YAML::Node& objects);

    /// @brief Parses a YAML node into a vector of Phases
    ///        Extracts each phase's name and its associated species (including optional properties).
    ///        Assumes the input YAML has already been validated for required structure and keys.
    /// @param objects YAML node representing phase list
    /// @return A vector of parsed Phases
    std::vector<types::Phase> ParsePhases(const YAML::Node& objects);

    /// @brief Parses a YAML node into reaction components
    /// @param object YAML node representing ReactionComponents
    /// @param key Key of the sequence to parse
    /// @return Vector of `types::ReactionComponent` with names, optional coefficients, and comments
    std::vector<types::ReactionComponent> ParseReactionComponents(const YAML::Node& object, const std::string& key);

    /// @brief Parses a single reaction component from a YAML node.
    ///        The parser performs no validation or error checking.
    /// @param object YAML node representing ReactionComponents
    /// @param key Key identifying the reaction component
    /// @return The parsed `types::ReactionComponent`, or a default-constructed one if none found
    types::ReactionComponent ParseReactionComponent(const YAML::Node& object, const std::string& key);

    /// @brief Parses a collection of YAML nodes into reaction objects
    ///        Iterates over the given YAML nodes, identifies the parser for each reaction type,
    ///        and populates a `types::Reactions` container with the parsed reactions.
    /// @param objects YAML node containing multiple reaction definitions
    /// @return A `types::Reactions` object with all successfully parsed reactions
    types::Reactions ParseReactions(const YAML::Node& objects);

    // @brief Parses a collection of YAML nodes into model objects
    ///        Iterates over the given YAML nodes, identifies the parser for each model type,
    ///        and populates a `types::Models` container with the parsed models.
    /// @param objects YAML node containing multiple model definitions
    /// @return A `types:: Models` object with all successfully parsed models
    types::Models ParseModels(const YAML::Node& objects);

  }  // namespace development
}  // namespace mechanism_configuration

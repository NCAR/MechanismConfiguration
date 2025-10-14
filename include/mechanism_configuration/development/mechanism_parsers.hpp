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

    // TODO(in progress) - Added 'V2' suffix to avoid the name collsion during the refactoring.
    //                     Once the refactoring is complete, the suffix will be removed
    types::ReactionComponent ParseReactionComponentV2(const YAML::Node& object);

    // TODO(in progress) - Added 'V2' suffix to avoid the name collsion during the refactoring.
    //                     Once the refactoring is complete, the suffix will be removed
    std::vector<types::ReactionComponent> ParseReactantsOrProductsV2(const std::string& key, const YAML::Node& object);
    
    // TODO (in progress) - Once the separating parsing from the validation for all reactions, 
    //                      This will be removed 
    std::pair<Errors, types::ReactionComponent> ParseReactionComponent(const YAML::Node& object);
    
    // TODO (in progress) - Once the separating parsing from the validation for all reactions, 
    //                      This will be removed 
    std::pair<Errors, std::vector<types::ReactionComponent>> ParseReactantsOrProducts(const std::string& key, const YAML::Node& object);

    std::pair<Errors, types::Reactions> ParseReactions(
      const YAML::Node& objects, 
      const std::vector<types::Species>& existing_species, 
      const std::vector<types::Phase>& existing_phases);

    std::pair<Errors, types::Models> ParseModels(const YAML::Node& objects, const std::vector<types::Phase>& existing_phases);


  }  // namespace development
}  // namespace mechanism_configuration

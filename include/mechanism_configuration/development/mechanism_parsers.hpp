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
    /// @param objects - YAML node representing species list
    /// @return vector of parsed species
    std::vector<types::Species> ParseSpecies(const YAML::Node& objects);

    std::pair<Errors, std::vector<types::Phase>> ParsePhases(const YAML::Node& objects, const std::vector<types::Species>& existing_species);

    std::pair<Errors, types::ReactionComponent> ParseReactionComponent(const YAML::Node& object);

    std::pair<Errors, std::vector<types::ReactionComponent>> ParseReactantsOrProducts(const std::string& key, const YAML::Node& object);

    std::pair<Errors, types::Reactions>
    ParseReactions(const YAML::Node& objects, const std::vector<types::Species>& existing_species, const std::vector<types::Phase>& existing_phases);

    std::pair<Errors, types::Models> ParseModels(const YAML::Node& objects, const std::vector<types::Phase>& existing_phases);

  }  // namespace development
}  // namespace mechanism_configuration

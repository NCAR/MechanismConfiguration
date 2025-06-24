// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/v1/model_types.hpp>
#include <mechanism_configuration/v1/reaction_types.hpp>
#include <mechanism_configuration/v1/types.hpp>

#include <yaml-cpp/yaml.h>

#include <vector>
#include <utility>

namespace mechanism_configuration
{
  namespace v1
  {
    std::pair<Errors, std::vector<types::Species>> ParseSpecies(const YAML::Node& objects);

    std::pair<Errors, std::vector<types::Phase>> ParsePhases(const YAML::Node& objects, const std::vector<v1::types::Species> existing_species);

    std::pair<Errors, types::ReactionComponent> ParseReactionComponent(const YAML::Node& object);

    std::pair<Errors, std::vector<types::ReactionComponent>> ParseReactantsOrProducts(const std::string& key, const YAML::Node& object);

    std::pair<Errors, types::Reactions>
    ParseReactions(const YAML::Node& objects, const std::vector<types::Species>& existing_species, const std::vector<types::Phase>& existing_phases);

    std::pair<Errors, types::Models> ParseModels(const YAML::Node& objects, const std::vector<types::Phase>& existing_phases);

  }  // namespace v1
}  // namespace mechanism_configuration

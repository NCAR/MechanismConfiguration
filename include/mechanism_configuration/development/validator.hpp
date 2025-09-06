// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration
{
  namespace development
  {

    Errors ValidateSpecies(const YAML::Node& species_list);
    // Errors ValidatePhases(const std::vector<types::Phase>& phases, const std::vector<types::Species>& species);
    // Errors ValidateModels(const std::vector<types::Model>& models, const std::vector<types::Phase>& phases);
    // Errors ValidateReactions(const std::vector<types::Reaction>& reactions, const std::vector<types::Species>& species, const std::vector<types::Phase>& phases);
    // Errors ValidateVersion(const Version& version);
    // Errors Validate(const types::Mechanism& mechanism);


  }  // namespace development
}  // namespace mechanism_configuration
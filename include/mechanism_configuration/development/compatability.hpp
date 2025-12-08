// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/development/model_types.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/types.hpp>
#include <mechanism_configuration/mechanism.hpp>

#include <yaml-cpp/yaml.h>

#include <string>
#include <vector>

namespace mechanism_configuration
{
  namespace development
  {
    // Apply backward compatibility transformations to the YAML node
    void BackwardCompatibleSpeciesName(YAML::Node& node);

  }  // namespace development
}  // namespace mechanism_configuration
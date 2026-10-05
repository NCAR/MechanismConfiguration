// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/types/emissions.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration::v1
{
  /// @brief Parses a YAML node into an EmissionsConfig.
  ///        The input must be validated using CheckEmissionsSchema().
  ///        This function assumes the structure and types are correct.
  /// @param emissions_node YAML node for the `emissions` key
  /// @return The parsed EmissionsConfig
  types::EmissionsConfig ParseEmissions(const YAML::Node& emissions_node);

}  // namespace mechanism_configuration::v1

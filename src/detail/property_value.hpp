// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/types/property_value.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration
{
  /// @brief Converts a YAML node to a PropertyValue, keeping the structure of sequences and maps.
  ///        Plain scalars are typed with the YAML 1.2 core schema (null, bool, int, float, string).
  ///        Quoted scalars are always strings.
  types::PropertyValue ToPropertyValue(const YAML::Node& node);
}  // namespace mechanism_configuration

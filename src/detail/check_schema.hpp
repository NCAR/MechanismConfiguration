// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

#include <string>
#include <string_view>
#include <vector>

namespace mechanism_configuration
{
  /// @brief Checks the keys of a YAML object against a schema.
  /// @param object The YAML node to validate
  /// @param required_keys Keys that must all be present
  /// @param optional_keys Keys that may be present
  /// @param exactly_one_of Groups of mutually-exclusive keys; exactly one member of each
  ///        group must be present (zero -> RequiredKeyNotFound, more than one ->
  ///        MutuallyExclusiveOption). Group members are treated as allowed keys.
  /// @return A collection of schema errors; empty if the object conforms.
  Errors CheckSchema(
      const YAML::Node& object,
      const std::vector<std::string_view>& required_keys,
      const std::vector<std::string_view>& optional_keys,
      const std::vector<std::vector<std::string_view>>& exactly_one_of = {});
}  // namespace mechanism_configuration

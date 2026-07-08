// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <detail/v1/keys.hpp>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace mechanism_configuration::v1
{
  /// @brief Ensures a YAML node is treated as a sequence
  /// @param node The YAML node to convert
  /// @return A YAML sequence node containing the original node(s)
  YAML::Node AsSequence(const YAML::Node& node);

  /// @brief Extracts a YAML node's source location, for error messages that carry line:col.
  ErrorLocation LocationOf(const YAML::Node& node);

  void AppendFilePath(const std::string& config_path, Errors& errors);

  std::unordered_map<std::string, std::string> GetComments(const YAML::Node& object);

  /// @brief Reads a named entry's name, accepting either a bare-string shorthand or an object
  ///        keyed by the canonical `name` or the legacy `species name` alias (v1 files). Used for
  ///        species, phase-species, and reaction component entries.
  /// @note Assumes the entry has already been validated to contain exactly one of them.
  std::string GetComponentName(const YAML::Node& component);

}  // namespace mechanism_configuration::v1

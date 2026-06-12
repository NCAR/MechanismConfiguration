// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/types.hpp>
#include <mechanism_configuration/validation.hpp>
#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mechanism_configuration
{
  namespace v1
  {
    /// @brief Ensures a YAML node is treated as a sequence
    /// @param node The YAML node to convert
    /// @return A YAML sequence node containing the original node(s)
    YAML::Node AsSequence(const YAML::Node& node);

    void AppendFilePath(const std::string& config_path, Errors& errors);

    std::unordered_map<std::string, std::string> GetComments(const YAML::Node& object);

    /// @brief Reads a reaction component's species reference, accepting either the
    ///        canonical `name` key or the legacy `species name` alias (v1 files).
    /// @note Assumes the component has already been validated to contain exactly one of them.
    std::string GetReactionComponentName(const YAML::Node& component);

  }  // namespace v1
}  // namespace mechanism_configuration

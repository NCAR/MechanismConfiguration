// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/types.hpp>

#include <yaml-cpp/yaml.h>

#include <string>
#include <vector>

namespace mechanism_configuration::development
{
  class Parser
  {
   public:
    Parser() = default;

    /// @brief Load a YAML file and return its root node
    /// @throws std::runtime_error If the file is missing, not a regular file, or cannot be parsed
    YAML::Node FileToYaml(const std::filesystem::path& config_path);

    /// @brief Validates mechanism YAML node.
    /// @param object The YAML node to validate
    /// @param read_from_config_file Whether to use the provided config path or the default
    /// @return A collection of validation errors; empty if the node is valid
    Errors Validate(const YAML::Node& object, bool read_from_config_file = true);

    /// @brief Constructs a Mechanism object from the provided YAML node
    /// @note Must be called only after successful validation
    Mechanism Parse(const YAML::Node& object);

    inline void SetConfigPath(const std::string& config_path)
    {
      config_path_ = config_path;
    }

   private:
    std::string config_path_;

    inline void SetDefaultConfigPath()
    {
      config_path_ = "";
    }
  };
}  // namespace mechanism_configuration::development
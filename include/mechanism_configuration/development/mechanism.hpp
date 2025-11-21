// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/development/model_types.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/types.hpp>
#include <mechanism_configuration/mechanism.hpp>

// TODO
#include <mechanism_configuration/parser_result.hpp>

#include <string>
#include <vector>

namespace mechanism_configuration {
namespace development {
namespace types {

/// @brief Represents a full mechanism definition
struct Mechanism : public ::mechanism_configuration::Mechanism
{
  /// @brief Mechanism name (optional)
  std::string name;
  /// @brief Species list
  std::vector<Species> species;
  /// @brief Phases list
  std::vector<Phase> phases;
  /// @brief Represents a collection of different reaction types, each stored in a vector
  ///        corresponding to a specific mechanism
  Reactions reactions;
  /// @brief Represents a general model (optional)
  Models models;
};

}  // namespace types

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
  Errors Validate(const YAML::Node& object, bool read_from_config_file=true);

  /// @brief Constructs a Mechanism object from the provided YAML node
  /// @note Must be called only after successful validation
  types::Mechanism Parse(const YAML::Node& object);

  inline void SetConfigPath(const std::string& config_path) { config_path_ = config_path; }

private:
  std::string config_path_;
  
  inline void SetDefaultConfigPath() { config_path_ = ""; }

};

}  // namespace development
}  // namespace mechanism_configuration
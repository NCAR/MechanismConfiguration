// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/types.hpp>
#include <mechanism_configuration/validate.hpp>

#include <yaml-cpp/yaml.h>

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace mechanism_configuration::v1
{
  /// @brief Extracts a located semantics::Input from a fully-resolved (inline) v1 YAML node, so
  ///        the version-neutral ValidateSemantics can run the semantic checks with line:col.
  semantics::Input BuildSemanticInput(const YAML::Node& object);

  class Parser
  {
   public:
    Parser() = default;

    /// @brief Load a YAML file and return its root node
    /// @throws std::runtime_error If the file is missing, not a regular file, or cannot be parsed
    YAML::Node FileToYaml(const std::filesystem::path& config_path);

    /// @brief Loads a configuration file and resolves any file-list sections into a single
    ///        inline node. A v1.1+ configuration may organize species/phases/reactions across
    ///        multiple files via a `{ files: [...] }` object; this merges them inline so the
    ///        resulting node can be validated and parsed normally. Inline sections pass through.
    /// @param config_path Path to the main configuration file.
    /// @return The combined inline node, or the collected structural / file-loading errors.
    std::expected<YAML::Node, Errors> ResolveFileConfig(const std::filesystem::path& config_path);

    /// @brief Validates a mechanism YAML node (structure + semantics) and, if valid, builds
    ///        the Mechanism. Validation always runs first, so a caller cannot build from an
    ///        unvalidated node.
    /// @param object The YAML node to validate and parse
    /// @param read_from_config_file Whether to keep the loaded config path (for error
    ///        message prefixes) or reset it to the default
    /// @return The parsed Mechanism, or all structural and semantic errors encountered
    std::expected<Mechanism, Errors> Parse(const YAML::Node& object, bool read_from_config_file = true);

    inline void SetConfigPath(const std::string& config_path)
    {
      config_path_ = config_path;
    }

   private:
    std::string config_path_;

    /// @brief Checks the structural schema and semantic invariants of a mechanism YAML node.
    Errors CheckSchema(const YAML::Node& object, bool read_from_config_file = true);

    /// @brief Constructs a Mechanism from an already-validated node.
    Mechanism Build(const YAML::Node& object);

    inline void SetDefaultConfigPath()
    {
      config_path_ = "";
    }
  };
}  // namespace mechanism_configuration::v1
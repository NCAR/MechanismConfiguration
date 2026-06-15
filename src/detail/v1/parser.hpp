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

    /// @brief Parse a v1 mechanism from a configuration file: resolves any file-list sections
    ///        (v1.1+ `{ files: [...] }`) into a single document, validates it, and builds the
    ///        Mechanism. This is the file entry point — no separate resolve/validate step.
    /// @param config_path Path to the main configuration file.
    /// @return The parsed Mechanism, or all structural / file-loading / semantic errors.
    std::expected<Mechanism, Errors> Parse(const std::filesystem::path& config_path);

    /// @brief Parse a v1 mechanism from an in-memory YAML/JSON document string.
    /// @param content The document contents (not a file path).
    /// @return The parsed Mechanism, or all structural and semantic errors.
    std::expected<Mechanism, Errors> Parse(const std::string& content);

    /// @brief Validate an already-loaded YAML node (structure + semantics) and build the
    ///        Mechanism. Validation always runs first, so a caller cannot build from an
    ///        unvalidated node.
    /// @param object The YAML node to validate and parse
    /// @param read_from_config_file Whether to keep the loaded config path (for error
    ///        message prefixes) or reset it to the default
    /// @return The parsed Mechanism, or all structural and semantic errors encountered
    std::expected<Mechanism, Errors> Parse(const YAML::Node& object, bool read_from_config_file = true);

   private:
    std::string config_path_;

    /// @brief Resolves a configuration file's file-list sections into a single inline node.
    std::expected<YAML::Node, Errors> ResolveFileConfig(const std::filesystem::path& config_path);

    /// @brief Checks the structural schema and semantic invariants of a mechanism YAML node.
    Errors CheckSchema(const YAML::Node& object, bool read_from_config_file = true);

    /// @brief Constructs a Mechanism from an already-validated node.
    Mechanism Build(const YAML::Node& object);

    inline void SetConfigPath(const std::string& config_path)
    {
      config_path_ = config_path;
    }

    inline void SetDefaultConfigPath()
    {
      config_path_ = "";
    }
  };
}  // namespace mechanism_configuration::v1
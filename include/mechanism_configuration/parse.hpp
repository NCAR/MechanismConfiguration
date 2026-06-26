// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/mechanism.hpp>

#include <expected>
#include <filesystem>

namespace mechanism_configuration
{
  /// @brief Parse a mechanism configuration file, dispatching on its version.
  /// @param config_path Path to a configuration file, or a directory of version-0 CAMP files.
  /// @return The parsed Mechanism, or all structural and semantic errors encountered.
  std::expected<Mechanism, Errors> Parse(const std::filesystem::path& config_path);

  /// @brief Parse a mechanism configuration from an in-memory YAML or JSON string.
  /// @param content The document contents (not a file path).
  /// @return The parsed Mechanism, or all structural and semantic errors encountered.
  std::expected<Mechanism, Errors> Parse(const std::string& content);
}  // namespace mechanism_configuration
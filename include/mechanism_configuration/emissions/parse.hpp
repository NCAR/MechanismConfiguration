// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/emissions/types.hpp>
#include <mechanism_configuration/errors.hpp>

#include <expected>
#include <filesystem>
#include <string>

namespace mechanism_configuration::emissions::v1
{
  /// @brief Parse an emissions configuration file.
  /// @param config_path Path to a YAML emissions configuration file.
  /// @return The parsed EmissionsConfig, or all structural and semantic errors encountered.
  std::expected<types::EmissionsConfig, Errors> ParseEmissions(const std::filesystem::path& config_path);

  /// @brief Parse an emissions configuration from an in-memory YAML string.
  /// @param content The YAML document contents (not a file path).
  /// @return The parsed EmissionsConfig, or all structural and semantic errors encountered.
  std::expected<types::EmissionsConfig, Errors> ParseEmissions(const std::string& content);

}  // namespace mechanism_configuration::emissions::v1

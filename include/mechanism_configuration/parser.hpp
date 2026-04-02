// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/parser_result.hpp>

#include <filesystem>

namespace mechanism_configuration
{
  struct VersionInfo
  {
    unsigned int version;  // major version
    Errors errors;
  };

  class UniversalParser
  {
   public:
    /// @brief Extracts the major version from the given configuration file
    /// @return VersionInfo containing the version and validation errors
    VersionInfo GetVersion(const std::filesystem::path& config_path);

    /// @brief Parses a configuration file using the appropriate versioned parser.
    ///        Determines the configuration version and calls the corresponding parser
    ///        implementation. If the version field is missing, the function falls
    ///        back to the version 0 parser.
    /// @param config_path Path to the YAML configuration file
    /// @return ParserResult containing the parsed GlobalMechanism on success,
    ///         or a list of accumulated errors on failure
    ParserResult<GlobalMechanism> Parse(const std::filesystem::path& config_path);
  };
}  // namespace mechanism_configuration
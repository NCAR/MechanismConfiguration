// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/mechanism.hpp>

#include <filesystem>
#include <optional>
#include <string>

namespace mechanism_configuration
{
  /// @brief The result of a parse: as much of the mechanism as the parser can build, and all the
  ///        errors that it finds.
  struct ParseResult
  {
    /// @brief The mechanism. It is empty only when the parser cannot read the configuration, for
    ///        example when the file does not exist or the version is not supported. When `errors`
    ///        is not empty, the mechanism does not have the items that cannot be built, and it can
    ///        refer to species or phases that it does not define.
    std::optional<Mechanism> mechanism;
    /// @brief The schema errors, the items that are left out, and the semantic errors.
    Errors errors;

    /// @brief True when the mechanism is complete and valid: it exists and there are no errors.
    explicit operator bool() const
    {
      return mechanism.has_value() && errors.empty();
    }
  };

  /// @brief Parse a mechanism configuration file, dispatching on its version. A species, phase, or
  ///        reaction that cannot be built is left out, and the parser builds the rest. A v1 aerosol
  ///        or emissions section that cannot be built is left out as a whole.
  /// @param config_path Path to a configuration file (a std::string converts implicitly),
  ///        or a directory of version-0 CAMP files.
  /// @return The mechanism and all the errors. Use the mechanism in a model only when the result
  ///         converts to true.
  ParseResult Parse(const std::filesystem::path& config_path);
  ParseResult ParseFromString(const std::string& config);
}  // namespace mechanism_configuration
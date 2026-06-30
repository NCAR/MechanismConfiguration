// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace mechanism_configuration
{
  struct ErrorLocation
  {
    int line, column;

    /// @brief Constructor that adjusts 0-based indices to 1-based
    /// @param line_number 0-based line number (from YAML::Mark)
    /// @param column_number 0-based column number (from YAML::Mark)
    ErrorLocation(int line_number, int column_number)
        : line(line_number + 1),
          column(column_number + 1)
    {
    }
  };

  enum class ErrorCode
  {
    Success,
    None,
    InvalidKey,
    UnknownKey,
    InvalidFilePath,
    ObjectTypeNotFound,
    RequiredKeyNotFound,
    MutuallyExclusiveOption,
    DuplicateSpeciesDetected,
    DuplicatePhasesDetected,
    DuplicateSpeciesInPhaseDetected,
    PhaseRequiresUnknownSpecies,
    ReactionRequiresUnknownSpecies,
    UnknownSpecies,
    UnknownPhase,
    RequestedSpeciesNotRegisteredInPhase,
    TooManyReactionComponents,
    InvalidVersion,
    MissingVersionField,
    InvalidType,
    UnknownType,
    FileNotFound,
    UnexpectedError,
    EmptyObject,
  };

  std::string ErrorCodeToString(const ErrorCode& status);

  // For Google Test printing
  inline void PrintTo(const ErrorCode& status, std::ostream* os)
  {
    *os << ErrorCodeToString(status);
  }

  using Errors = std::vector<std::pair<ErrorCode, std::string>>;
}  // namespace mechanism_configuration
// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <optional>
#include <string>
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
    // Emissions-specific error codes
    DuplicateInventoryDetected,
    DuplicateSpeciesMapDetected,
    DuplicateSourceDetected,
    DuplicateCategoryHierarchy,
    SourceRequiresUnknownInventory,
    SourceRequiresUnknownSpeciesMap,
    SpeciesMapScalingExceedsOne,
    OnlineSourcesNotSupported,
    UnsupportedRegriddingType,
    UnsupportedVerticalInjection,
  };

  std::string ErrorCodeToString(const ErrorCode& status);

  /// @brief An error found when a configuration is parsed or validated
  struct Error
  {
    ErrorCode code;
    /// @brief The full message, with the "file:line:col" prefix when the location is known
    std::string message;
    /// @brief A JSON Pointer (RFC 6901) to the object in the configuration document that
    ///        causes the error. Empty when the error is not about one object, or when the
    ///        mechanism has no document (for example, a Mechanism built in code).
    std::optional<std::string> path{};
  };

  using Errors = std::vector<Error>;
}  // namespace mechanism_configuration
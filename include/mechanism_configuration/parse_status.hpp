// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <ostream>
#include <string>

namespace mechanism_configuration
{
  enum class ConfigParseStatus
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
    InvalidIonPair,
    InvalidVersion,
    MissingVersionField,
    InvalidParameterNumber,
    InvalidType,
    UnknownType,
    FileNotFound,
    UnexpectedError,
    EmptyObject,
  };

  std::string configParseStatusToString(const ConfigParseStatus& status);

  // For Google Test printing
  inline void PrintTo(const ConfigParseStatus& status, std::ostream* os)
  {
    *os << configParseStatusToString(status);
  }
}  // namespace mechanism_configuration

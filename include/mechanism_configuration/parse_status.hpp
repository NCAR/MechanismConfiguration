// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

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
    EmptyObject,
    ObjectTypeNotFound,
    RequiredKeyNotFound,
    MutuallyExclusiveOption,
    DuplicateSpeciesDetected,
    DuplicatePhasesDetected,
    DuplicateSpeciesInPhaseDetected,
    PhaseRequiresUnknownSpecies,    // TODO: remove
    ReactionRequiresUnknownSpecies, // TODO: remove
    UnknownSpecies,
    UnknownPhase,
    RequestedSpeciesNotRegisteredInPhase,
    TooManyReactionComponents,
    InvalidIonPair,
    InvalidVersion,
    UnknownType,
    FileNotFound
  };

  std::string configParseStatusToString(const ConfigParseStatus &status);
}  // namespace mechanism_configuration

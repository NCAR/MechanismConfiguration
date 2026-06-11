// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/errors.hpp>

namespace mechanism_configuration
{
  std::string ErrorCodeToString(const ErrorCode& status)
  {
    switch (status)
    {
      case ErrorCode::Success: return "Success";
      case ErrorCode::None: return "None";
      case ErrorCode::InvalidKey: return "InvalidKey";
      case ErrorCode::UnknownKey: return "UnknownKey";
      case ErrorCode::InvalidFilePath: return "InvalidFilePath";
      case ErrorCode::ObjectTypeNotFound: return "ObjectTypeNotFound";
      case ErrorCode::RequiredKeyNotFound: return "RequiredKeyNotFound";
      case ErrorCode::MutuallyExclusiveOption: return "MutuallyExclusiveOption";
      case ErrorCode::DuplicateSpeciesDetected: return "DuplicateSpeciesDetected";
      case ErrorCode::DuplicatePhasesDetected: return "DuplicatePhasesDetected";
      case ErrorCode::DuplicateSpeciesInPhaseDetected: return "DuplicateSpeciesInPhaseDetected";
      case ErrorCode::PhaseRequiresUnknownSpecies: return "PhaseRequiresUnknownSpecies";
      case ErrorCode::ReactionRequiresUnknownSpecies: return "ReactionRequiresUnknownSpecies";
      case ErrorCode::UnknownSpecies: return "UnknownSpecies";
      case ErrorCode::UnknownPhase: return "UnknownPhase";
      case ErrorCode::RequestedSpeciesNotRegisteredInPhase: return "RequestedSpeciesNotRegisteredInPhase";
      case ErrorCode::TooManyReactionComponents: return "TooManyReactionComponents";
      case ErrorCode::InvalidIonPair: return "InvalidIonPair";
      case ErrorCode::InvalidVersion: return "InvalidVersion";
      case ErrorCode::MissingVersionField: return "MissingVersionField";
      case ErrorCode::InvalidParameterNumber: return "InvalidParameterNumber";
      case ErrorCode::InvalidType: return "InvalidType";
      case ErrorCode::UnknownType: return "UnknownType";
      case ErrorCode::FileNotFound: return "FileNotFound";
      case ErrorCode::UnexpectedError: return "UnexpectedError";
      case ErrorCode::EmptyObject: return "EmptyObject";
      default: return "Unknown";
    }
  }
}  // namespace mechanism_configuration
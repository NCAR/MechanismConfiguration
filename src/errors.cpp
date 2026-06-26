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
      case ErrorCode::InvalidVersion: return "InvalidVersion";
      case ErrorCode::MissingVersionField: return "MissingVersionField";
      case ErrorCode::InvalidType: return "InvalidType";
      case ErrorCode::UnknownType: return "UnknownType";
      case ErrorCode::FileNotFound: return "FileNotFound";
      case ErrorCode::UnexpectedError: return "UnexpectedError";
      case ErrorCode::EmptyObject: return "EmptyObject";
      case ErrorCode::DuplicateInventoryDetected: return "DuplicateInventoryDetected";
      case ErrorCode::DuplicateSpeciesMapDetected: return "DuplicateSpeciesMapDetected";
      case ErrorCode::DuplicateSourceDetected: return "DuplicateSourceDetected";
      case ErrorCode::DuplicateCategoryHierarchy: return "DuplicateCategoryHierarchy";
      case ErrorCode::SourceRequiresUnknownInventory: return "SourceRequiresUnknownInventory";
      case ErrorCode::SourceRequiresUnknownSpeciesMap: return "SourceRequiresUnknownSpeciesMap";
      case ErrorCode::SpeciesMapScalingExceedsOne: return "SpeciesMapScalingExceedsOne";
      case ErrorCode::OnlineSourcesNotSupported: return "OnlineSourcesNotSupported";
      case ErrorCode::UnsupportedRegriddingType: return "UnsupportedRegriddingType";
      case ErrorCode::UnsupportedVerticalInjection: return "UnsupportedVerticalInjection";
      default: return "Unknown";
    }
  }
}  // namespace mechanism_configuration
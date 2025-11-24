// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/parse_status.hpp>

namespace mechanism_configuration
{
  std::string configParseStatusToString(const ConfigParseStatus& status)
  {
    switch (status)
    {
      case ConfigParseStatus::Success: return "Success";
      case ConfigParseStatus::None: return "None";
      case ConfigParseStatus::InvalidKey: return "InvalidKey";
      case ConfigParseStatus::UnknownKey: return "UnknownKey";
      case ConfigParseStatus::InvalidFilePath: return "InvalidFilePath";
      case ConfigParseStatus::ObjectTypeNotFound: return "ObjectTypeNotFound";
      case ConfigParseStatus::RequiredKeyNotFound: return "RequiredKeyNotFound";
      case ConfigParseStatus::MutuallyExclusiveOption: return "MutuallyExclusiveOption";
      case ConfigParseStatus::DuplicateSpeciesDetected: return "DuplicateSpeciesDetected";
      case ConfigParseStatus::DuplicatePhasesDetected: return "DuplicatePhasesDetected";
      case ConfigParseStatus::DuplicateSpeciesInPhaseDetected: return "DuplicateSpeciesInPhaseDetected";
      case ConfigParseStatus::PhaseRequiresUnknownSpecies: return "PhaseRequiresUnknownSpecies";
      case ConfigParseStatus::ReactionRequiresUnknownSpecies: return "ReactionRequiresUnknownSpecies";
      case ConfigParseStatus::UnknownSpecies: return "UnknownSpecies";
      case ConfigParseStatus::UnknownPhase: return "UnknownPhase";
      case ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase: return "RequestedSpeciesNotRegisteredInPhase";
      case ConfigParseStatus::TooManyReactionComponents: return "TooManyReactionComponents";
      case ConfigParseStatus::InvalidIonPair: return "InvalidIonPair";
      case ConfigParseStatus::InvalidVersion: return "InvalidVersion";
      case ConfigParseStatus::MissingVersionField: return "MissingVersionField";
      case ConfigParseStatus::InvalidParameterNumber: return "InvalidParameterNumber";
      case ConfigParseStatus::InvalidType: return "InvalidType";
      case ConfigParseStatus::UnknownType: return "UnknownType";
      case ConfigParseStatus::FileNotFound: return "FileNotFound";
      case ConfigParseStatus::UnexpectedError: return "UnexpectedError";
      case ConfigParseStatus::EmptyObject: return "EmptyObject";
      default: return "Unknown";
    }
  }
}  // namespace mechanism_configuration
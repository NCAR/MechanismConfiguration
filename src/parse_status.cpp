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
      case ConfigParseStatus::PhaseRequiresUnknownSpecies: return "PhaseRequiresUnknownSpecies";
      case ConfigParseStatus::ReactionRequiresUnknownSpecies: return "ReactionRequiresUnknownSpecies";
      case ConfigParseStatus::UnknownPhase: return "UnknownPhase";
      case ConfigParseStatus::RequestedAqeuousSpeciesNotIncludedInAqueousPhase: return "RequestedAqeuousSpeciesNotIncludedInAqueousPhase";
      case ConfigParseStatus::TooManyReactionComponents: return "TooManyReactionComponents";
      case ConfigParseStatus::InvalidIonPair: return "InvalidIonPair";
      case ConfigParseStatus::InvalidVersion: return "InvalidVersion";
      case ConfigParseStatus::UnknownType: return "UnknownType";
      case ConfigParseStatus::FileNotFound: return "FileNotFound";
      default: return "Unknown";
    }
  }
}  // namespace mechanism_configuration
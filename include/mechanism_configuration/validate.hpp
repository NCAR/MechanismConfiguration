// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/mechanism.hpp>

#include <optional>
#include <string>
#include <vector>

namespace mechanism_configuration
{
  // Intermediate over which all semantic validation runs. Both a parsed document and an
  // in-code Mechanism produce one of these and hand it to ValidateSemantics, so the rules
  // live in exactly one place. A parsed document fills in source locations (so errors carry
  // line:col); an in-code Mechanism leaves them empty. The domain types stay location-free.
  namespace semantics
  {
    struct NamedRef
    {
      std::string name;
      std::optional<ErrorLocation> location;
    };

    struct PhaseRef
    {
      std::string name;
      std::vector<NamedRef> species;
      std::optional<ErrorLocation> location;
    };

    struct ReactionRef
    {
      std::string type;
      std::string phase;
      std::vector<NamedRef> reactants;  // must exist and be registered in `phase`
      std::vector<NamedRef> products;   // must exist; may belong to any phase
      std::optional<ErrorLocation> location;
    };

    struct SpeciesMappingRef
    {
      std::string inventory_species;
      std::string mechanism_species;
      double scaling_factor{ 1.0 };
    };

    struct SpeciesMapRef
    {
      std::string name;
      std::vector<SpeciesMappingRef> mappings;
      std::optional<ErrorLocation> location;
    };

    struct SourceRef
    {
      std::string name;
      NamedRef inventory;    // .name = referenced inventory name, .location = reference site
      NamedRef species_map;  // .name = referenced species-map name, .location = reference site
      int category{ 0 };
      int hierarchy{ 1 };
      std::optional<ErrorLocation> location;  // location of the source entry itself
    };

    struct EmissionsRef
    {
      std::vector<NamedRef> inventories;
      std::vector<SpeciesMapRef> species_maps;
      std::vector<SourceRef> sources;
    };

    struct Input
    {
      std::vector<NamedRef> species;
      std::vector<PhaseRef> phases;
      std::vector<ReactionRef> reactions;
      std::optional<EmissionsRef> emissions;  // nullopt only when there's no emissions at all
    };
  }  // namespace semantics

  /// @brief The single home for the mechanism's semantic rules. Errors include `line:col`
  ///        for any element whose source location was supplied.
  Errors ValidateSemantics(const semantics::Input& input);

  /// @brief Validates the semantic invariants of a canonical Mechanism, independent of how it
  ///        was produced (parsed from any version, or constructed in code). Convenience wrapper
  ///        that builds a location-free semantics::Input and calls ValidateSemantics.
  ///
  /// Structural/deserialization concerns (YAML keys, types, formatting) are the responsibility
  /// of the version-specific parsers and are not repeated here.
  Errors Validate(const Mechanism& mechanism);
}  // namespace mechanism_configuration

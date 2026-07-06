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

  // Intermediate over which the core semantic validation (species, phases, gas-phase reactions)
  // runs. Both a parsed document and an in-code Mechanism lower into one of these and hand it to
  // ValidateSemantics, so those rules live in exactly one place. A parsed document fills in source
  // locations (so errors carry line:col); an in-code Mechanism leaves them empty.
  //
  // Aerosol cross-references are validated separately by ValidateAerosolModel, which works on the
  // domain structs directly because it needs phase membership and optional-property values that
  // this name-only view cannot carry.
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

  /// @brief The single home for the core (species / phase / gas-phase reaction) semantic rules.
  ///        Errors include `line:col` for any element whose source location was supplied.
  Errors ValidateSemantics(const semantics::Input& input);

  /// @brief Validates a Mechanism's species, phases, and gas-phase reactions by
  ///        converting them to a location-free semantics::Input and running
  ///        ValidateSemantics.
  Errors ValidateGasModel(const Mechanism& mechanism);

  /// @brief Validates aerosol cross-references against the mechanism's species and phases.
  ///
  ///        Checks phase/species references, representation-keyed rate-constant maps, and
  ///        required definition-derived properties. These validations require full domain
  ///        information unavailable to ValidateSemantics. Returns no errors if the mechanism
  ///        has no aerosol section.
  Errors ValidateAerosolModel(const Mechanism& mechanism);

  /// @brief Validates the semantic invariants of a canonical Mechanism, regardless of whether it
  ///        was parsed or constructed in code. Combines ValidateGasModel and
  ///        ValidateAerosolModel, returning all validation errors.
  ///
  ///        Excludes structural/deserialization validation, which is handled by version-specific parsers.
  Errors Validate(const Mechanism& mechanism);

}  // namespace mechanism_configuration

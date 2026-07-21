// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "detail/semantics/reactions.hpp"

#include <optional>
#include <string>
#include <vector>

namespace mechanism_configuration::semantics
{
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

  struct EmissionsInput
  {
    std::vector<NamedRef> inventories;
    std::vector<SpeciesMapRef> species_maps;
    std::vector<SourceRef> sources;
  };

}  // namespace mechanism_configuration::semantics

namespace mechanism_configuration
{
  /// @brief Validates an emissions section: duplicate names, source cross-references (inventory /
  ///        species-map existence), duplicate (category, hierarchy) pairs, and species-map
  ///        scaling-factor sums. Independent of ValidateReactionsSemantics — emissions never
  ///        cross-references species or phases.
  Errors ValidateEmissionsSemantics(const semantics::EmissionsInput& input);
}  // namespace mechanism_configuration

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace mechanism_configuration::types
{

  struct SpeciesMapping
  {
    std::string inventory_species;
    std::string mechanism_species;
    double scaling_factor{ 1.0 };
  };

  struct SpeciesMap
  {
    std::string name;
    std::vector<SpeciesMapping> mappings;
  };

  struct Inventory
  {
    std::string name;
    std::string directory;
    std::string file_pattern;
    std::string convention;
  };

  enum class SourceMode
  {
    Offline,
  };

  enum class SourceType
  {
    Anthropogenic,
    Fire,
    Biogenic,
    Dust,
    SeaSalt,
    Lightning,
  };

  enum class TemporalInterpolation
  {
    Linear,
    Nearest,
    None,
  };

  enum class VerticalInjection
  {
    Surface,
  };

  struct SourceDescriptor
  {
    std::string name;
    SourceMode mode{ SourceMode::Offline };
    SourceType type{ SourceType::Anthropogenic };
    std::string inventory;
    std::string species_map;
    TemporalInterpolation temporal_interpolation{ TemporalInterpolation::Linear };
    VerticalInjection vertical_injection{ VerticalInjection::Surface };
    int category{ 0 };
    int hierarchy{ 1 };
    double scaling_factor{ 1.0 };
    std::string sector;
    std::unordered_map<std::string, std::string> unknown_properties;
  };

  enum class RegriddingType
  {
    None,
  };

  struct Regridding
  {
    RegriddingType type{ RegriddingType::None };
  };

  struct EmissionsConfig
  {
    std::vector<Inventory> inventories;
    std::vector<SpeciesMap> species_maps;
    Regridding regridding;
    std::vector<SourceDescriptor> sources;
  };

}  // namespace mechanism_configuration::types

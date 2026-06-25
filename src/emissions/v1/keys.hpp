// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

namespace mechanism_configuration::emissions::v1::keys
{
  // Top-level
  inline constexpr std::string_view kind = "kind";
  inline constexpr std::string_view kind_value = "emissions";
  inline constexpr std::string_view version = "version";
  inline constexpr std::string_view name = "name";
  inline constexpr std::string_view data_root = "data root";

  // Sections
  inline constexpr std::string_view inventories = "inventories";
  inline constexpr std::string_view species_maps = "species maps";
  inline constexpr std::string_view regridding = "regridding";
  inline constexpr std::string_view sources = "sources";

  // Inventory entry
  inline constexpr std::string_view directory = "directory";
  inline constexpr std::string_view file_pattern = "file pattern";
  inline constexpr std::string_view convention = "convention";

  // Species map entry
  inline constexpr std::string_view mappings = "mappings";
  inline constexpr std::string_view inventory_species = "inventory species";
  inline constexpr std::string_view mechanism_species = "mechanism species";
  inline constexpr std::string_view scaling_factor = "scaling factor";

  // Regridding entry
  inline constexpr std::string_view type = "type";
  inline constexpr std::string_view regridding_none = "none";
  inline constexpr std::string_view regridding_scrip = "scrip";

  // Source descriptor entry
  inline constexpr std::string_view mode = "mode";
  inline constexpr std::string_view source_type = "type";
  inline constexpr std::string_view inventory = "inventory";
  inline constexpr std::string_view species_map = "species map";
  inline constexpr std::string_view temporal_interpolation = "temporal interpolation";
  inline constexpr std::string_view vertical_injection = "vertical injection";
  inline constexpr std::string_view category = "category";
  inline constexpr std::string_view hierarchy = "hierarchy";
  inline constexpr std::string_view sector = "sector";

  // Mode values
  inline constexpr std::string_view mode_offline = "offline";
  inline constexpr std::string_view mode_online = "online";

  // Source type values
  inline constexpr std::string_view type_anthropogenic = "anthropogenic";
  inline constexpr std::string_view type_fire = "fire";
  inline constexpr std::string_view type_biogenic = "biogenic";
  inline constexpr std::string_view type_dust = "dust";
  inline constexpr std::string_view type_sea_salt = "sea salt";
  inline constexpr std::string_view type_lightning = "lightning";

  // Temporal interpolation values
  inline constexpr std::string_view interp_linear = "linear";
  inline constexpr std::string_view interp_nearest = "nearest";
  inline constexpr std::string_view interp_none = "none";

  // Vertical injection values
  inline constexpr std::string_view inject_surface = "surface";
  inline constexpr std::string_view inject_plume = "plume";

}  // namespace mechanism_configuration::emissions::v1::keys

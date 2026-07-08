// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/emissions/parsers.hpp"

#include "detail/v1/emissions/keys.hpp"
#include "detail/v1/keys.hpp"
#include "detail/v1/utils.hpp"

#include <string>

namespace mechanism_configuration::v1
{
  namespace
  {
    types::SourceType ParseSourceType(const std::string& s)
    {
      if (s == std::string(keys::type_fire))
        return types::SourceType::Fire;
      if (s == std::string(keys::type_biogenic))
        return types::SourceType::Biogenic;
      if (s == std::string(keys::type_dust))
        return types::SourceType::Dust;
      if (s == std::string(keys::type_sea_salt))
        return types::SourceType::SeaSalt;
      if (s == std::string(keys::type_lightning))
        return types::SourceType::Lightning;
      return types::SourceType::Anthropogenic;
    }

    types::TemporalInterpolation ParseTemporalInterpolation(const std::string& s)
    {
      if (s == std::string(keys::interp_nearest))
        return types::TemporalInterpolation::Nearest;
      if (s == std::string(keys::interp_none))
        return types::TemporalInterpolation::None;
      return types::TemporalInterpolation::Linear;
    }
  }  // namespace

  types::EmissionsConfig ParseEmissions(const YAML::Node& node)
  {
    types::EmissionsConfig config;

    if (node[std::string(keys::inventories)])
    {
      for (const auto& item : node[std::string(keys::inventories)])
      {
        types::Inventory inv;
        inv.name = item[std::string(keys::name)].as<std::string>();
        inv.directory = item[std::string(keys::directory)].as<std::string>();
        inv.file_pattern = item[std::string(keys::file_pattern)].as<std::string>();
        inv.convention = item[std::string(keys::convention)].as<std::string>();
        config.inventories.push_back(std::move(inv));
      }
    }

    if (node[std::string(keys::species_maps)])
    {
      for (const auto& item : node[std::string(keys::species_maps)])
      {
        types::SpeciesMap smap;
        smap.name = item[std::string(keys::name)].as<std::string>();
        for (const auto& mapping_node : item[std::string(keys::mappings)])
        {
          types::SpeciesMapping m;
          m.inventory_species = mapping_node[std::string(keys::inventory_species)].as<std::string>();
          m.mechanism_species = mapping_node[std::string(keys::mechanism_species)].as<std::string>();
          if (mapping_node[std::string(keys::scaling_factor)])
            m.scaling_factor = mapping_node[std::string(keys::scaling_factor)].as<double>();
          smap.mappings.push_back(std::move(m));
        }
        config.species_maps.push_back(std::move(smap));
      }
    }

    if (node[std::string(keys::regridding)] && node[std::string(keys::regridding)][std::string(keys::type)])
    {
      config.regridding.type = types::RegriddingType::None;
    }

    if (node[std::string(keys::sources)])
    {
      for (const auto& s : node[std::string(keys::sources)])
      {
        types::SourceDescriptor src;
        src.name = s[std::string(keys::name)].as<std::string>();
        src.type = ParseSourceType(s[std::string(keys::type)].as<std::string>());
        src.inventory = s[std::string(keys::inventory)].as<std::string>();
        src.species_map = s[std::string(keys::species_map)].as<std::string>();

        if (s[std::string(keys::temporal_interpolation)])
          src.temporal_interpolation =
              ParseTemporalInterpolation(s[std::string(keys::temporal_interpolation)].as<std::string>());

        if (s[std::string(keys::category)])
          src.category = s[std::string(keys::category)].as<int>();
        if (s[std::string(keys::hierarchy)])
          src.hierarchy = s[std::string(keys::hierarchy)].as<int>();
        if (s[std::string(keys::scaling_factor)])
          src.scaling_factor = s[std::string(keys::scaling_factor)].as<double>();
        if (s[std::string(keys::sector)])
          src.sector = s[std::string(keys::sector)].as<std::string>();

        src.unknown_properties = GetComments(s);
        config.sources.push_back(std::move(src));
      }
    }

    return config;
  }
}  // namespace mechanism_configuration::v1

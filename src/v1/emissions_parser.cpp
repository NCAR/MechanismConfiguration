// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/emissions_parser.hpp"

#include "detail/check_schema.hpp"
#include "detail/error_format.hpp"
#include "detail/v1/keys.hpp"
#include "detail/v1/utils.hpp"

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>

#include <string>
#include <string_view>
#include <vector>

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

    Errors CheckInventoriesSchema(const YAML::Node& inventories_node)
    {
      Errors errors;
      if (!inventories_node.IsSequence())
      {
        errors.push_back({ ErrorCode::InvalidType, "'inventories' must be a sequence" });
        return errors;
      }

      const std::vector<std::string_view> required_keys = {
        keys::name, keys::directory, keys::file_pattern, keys::convention
      };
      const std::vector<std::string_view> optional_keys = {};
      for (const auto& item : inventories_node)
      {
        auto schema_errors = CheckSchema(item, required_keys, optional_keys);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }
      return errors;
    }

    Errors CheckMappingsSchema(const YAML::Node& mappings_node)
    {
      Errors errors;
      if (!mappings_node.IsSequence())
      {
        errors.push_back({ ErrorCode::InvalidType, "'mappings' must be a sequence" });
        return errors;
      }

      const std::vector<std::string_view> required_keys = { keys::inventory_species, keys::mechanism_species };
      const std::vector<std::string_view> optional_keys = { keys::scaling_factor };
      for (const auto& item : mappings_node)
      {
        auto schema_errors = CheckSchema(item, required_keys, optional_keys);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }
      return errors;
    }

    Errors CheckSpeciesMapsSchema(const YAML::Node& species_maps_node)
    {
      Errors errors;
      if (!species_maps_node.IsSequence())
      {
        errors.push_back({ ErrorCode::InvalidType, "'species maps' must be a sequence" });
        return errors;
      }

      const std::vector<std::string_view> required_keys = { keys::name, keys::mappings };
      const std::vector<std::string_view> optional_keys = {};
      for (const auto& item : species_maps_node)
      {
        auto schema_errors = CheckSchema(item, required_keys, optional_keys);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());

        if (item[std::string(keys::mappings)])
        {
          auto mapping_errors = CheckMappingsSchema(item[std::string(keys::mappings)]);
          errors.insert(errors.end(), mapping_errors.begin(), mapping_errors.end());
        }
      }
      return errors;
    }

    Errors CheckSourcesSchema(const YAML::Node& sources_node)
    {
      Errors errors;
      if (!sources_node.IsSequence())
      {
        errors.push_back({ ErrorCode::InvalidType, "'sources' must be a sequence" });
        return errors;
      }

      const std::vector<std::string_view> required_keys = {
        keys::name, keys::mode, keys::type, keys::inventory, keys::species_map
      };
      const std::vector<std::string_view> optional_keys = { keys::temporal_interpolation,
                                                            keys::vertical_injection,
                                                            keys::category,
                                                            keys::hierarchy,
                                                            keys::scaling_factor,
                                                            keys::sector };

      for (const auto& item : sources_node)
      {
        auto schema_errors = CheckSchema(item, required_keys, optional_keys);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());

        // Fixed-enum-membership checks: the value is validated against a compile-time fixed
        // set, not a cross-reference to another user-supplied name, so this stays structural.
        if (item[std::string(keys::mode)])
        {
          const std::string mode_val = item[std::string(keys::mode)].as<std::string>();
          if (mode_val == std::string(keys::mode_online))
          {
            const ErrorLocation loc = LocationOf(item[std::string(keys::mode)]);
            errors.push_back({ ErrorCode::OnlineSourcesNotSupported,
                               mc_fmt::format("{} error: 'mode: online' is not supported in v1", loc) });
          }
          else if (mode_val != std::string(keys::mode_offline))
          {
            errors.push_back({ ErrorCode::UnknownType, mc_fmt::format("Unknown mode '{}'; expected 'offline'", mode_val) });
          }
        }

        if (item[std::string(keys::type)])
        {
          const std::string type_val = item[std::string(keys::type)].as<std::string>();
          static const std::vector<std::string_view> valid_types = { keys::type_anthropogenic, keys::type_fire,
                                                                     keys::type_biogenic,      keys::type_dust,
                                                                     keys::type_sea_salt,      keys::type_lightning };
          bool found = false;
          for (const auto& vt : valid_types)
            if (type_val == std::string(vt))
              found = true;
          if (!found)
            errors.push_back({ ErrorCode::UnknownType, mc_fmt::format("Unknown source type '{}'", type_val) });
        }

        if (item[std::string(keys::vertical_injection)])
        {
          const std::string vi_val = item[std::string(keys::vertical_injection)].as<std::string>();
          if (vi_val == std::string(keys::inject_plume))
          {
            const ErrorLocation loc = LocationOf(item[std::string(keys::vertical_injection)]);
            errors.push_back({ ErrorCode::UnsupportedVerticalInjection,
                               mc_fmt::format("{} error: 'vertical injection: plume' is not supported in v1", loc) });
          }
          else if (vi_val != std::string(keys::inject_surface))
          {
            errors.push_back(
                { ErrorCode::UnknownType, mc_fmt::format("Unknown vertical injection '{}'; expected 'surface'", vi_val) });
          }
        }
      }
      return errors;
    }
  }  // namespace

  Errors CheckEmissionsSchema(const YAML::Node& emissions_node)
  {
    Errors errors;

    if (emissions_node[std::string(keys::inventories)])
    {
      auto e = CheckInventoriesSchema(emissions_node[std::string(keys::inventories)]);
      errors.insert(errors.end(), e.begin(), e.end());
    }

    if (emissions_node[std::string(keys::species_maps)])
    {
      auto e = CheckSpeciesMapsSchema(emissions_node[std::string(keys::species_maps)]);
      errors.insert(errors.end(), e.begin(), e.end());
    }

    if (emissions_node[std::string(keys::regridding)])
    {
      const YAML::Node& rg_node = emissions_node[std::string(keys::regridding)];
      if (rg_node[std::string(keys::type)])
      {
        const std::string rg_type = rg_node[std::string(keys::type)].as<std::string>();
        if (rg_type != std::string(keys::regridding_none))
        {
          const ErrorLocation loc = LocationOf(rg_node[std::string(keys::type)]);
          errors.push_back(
              { ErrorCode::UnsupportedRegriddingType,
                mc_fmt::format(
                    "{} error: Unsupported regridding type '{}'; only 'none' is supported in v1", loc, rg_type) });
        }
      }
    }

    if (emissions_node[std::string(keys::sources)])
    {
      auto e = CheckSourcesSchema(emissions_node[std::string(keys::sources)]);
      errors.insert(errors.end(), e.begin(), e.end());
    }

    return errors;
  }

  types::EmissionsConfig ParseEmissionsSection(const YAML::Node& node)
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

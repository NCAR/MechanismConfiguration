// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/emissions/schema.hpp"

#include "detail/error_format.hpp"
#include "detail/schema.hpp"
#include "detail/v1/emissions/keys.hpp"
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

}  // namespace mechanism_configuration::v1

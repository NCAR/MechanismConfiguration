// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "emissions/v1/parser.hpp"

#include "emissions/v1/keys.hpp"

#include <mechanism_configuration/emissions/parse.hpp>
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/mechanism.hpp>

#include <yaml-cpp/yaml.h>

#include <cstdlib>
#include <filesystem>
#include <set>
#include <string>
#include <unordered_map>

namespace mechanism_configuration::emissions::v1
{
  namespace
  {
    void AppendFilePath(const std::string& config_path, Errors& errors)
    {
      if (config_path.empty())
        return;
      for (auto& [code, msg] : errors)
        if (msg.find(config_path) == std::string::npos)
          msg = config_path + ": " + msg;
    }

    // Parse all __ -prefixed keys as unknown properties.
    std::unordered_map<std::string, std::string> GetUnknownProperties(const YAML::Node& node)
    {
      std::unordered_map<std::string, std::string> props;
      for (const auto& kv : node)
      {
        std::string k = kv.first.as<std::string>();
        if (k.size() >= 2 && k[0] == '_' && k[1] == '_')
          props[k] = kv.second.as<std::string>();
      }
      return props;
    }

    types::SourceType ParseSourceType(const std::string& s)
    {
      if (s == keys::type_anthropogenic) return types::SourceType::Anthropogenic;
      if (s == keys::type_fire)          return types::SourceType::Fire;
      if (s == keys::type_biogenic)      return types::SourceType::Biogenic;
      if (s == keys::type_dust)          return types::SourceType::Dust;
      if (s == keys::type_sea_salt)      return types::SourceType::SeaSalt;
      if (s == keys::type_lightning)     return types::SourceType::Lightning;
      return types::SourceType::Anthropogenic;  // validated separately
    }

    types::TemporalInterpolation ParseTemporalInterpolation(const std::string& s)
    {
      if (s == keys::interp_linear)  return types::TemporalInterpolation::Linear;
      if (s == keys::interp_nearest) return types::TemporalInterpolation::Nearest;
      if (s == keys::interp_none)    return types::TemporalInterpolation::None;
      return types::TemporalInterpolation::Linear;  // validated separately
    }
  }  // namespace

  std::string Parser::ExpandEnvVars(const std::string& input, Errors& errors) const
  {
    std::string result;
    result.reserve(input.size());
    size_t i = 0;
    while (i < input.size())
    {
      if (input[i] == '$' && i + 1 < input.size() && input[i + 1] == '{')
      {
        size_t end = input.find('}', i + 2);
        if (end == std::string::npos)
        {
          errors.push_back({ ErrorCode::InvalidKey, "Unterminated ${...} in 'data root'" });
          return result;
        }
        const std::string var_expr = input.substr(i + 2, end - i - 2);
        const auto sep = var_expr.find(":-");
        std::string var_name, default_val;
        bool has_default = false;
        if (sep != std::string::npos)
        {
          var_name = var_expr.substr(0, sep);
          default_val = var_expr.substr(sep + 2);
          has_default = true;
        }
        else
        {
          var_name = var_expr;
        }
        const char* val = std::getenv(var_name.c_str());
        if (val)
        {
          result += val;
        }
        else if (has_default)
        {
          result += default_val;
        }
        else
        {
          errors.push_back({ ErrorCode::UndefinedEnvironmentVariable,
                             mc_fmt::format("Environment variable '{}' is not set and has no default", var_name) });
          return result;
        }
        i = end + 1;
      }
      else
      {
        result += input[i++];
      }
    }
    return result;
  }

  std::expected<types::EmissionsConfig, Errors> Parser::ValidateAndBuild(const YAML::Node& object)
  {
    try
    {
      Errors errors;

      // ── kind ──────────────────────────────────────────────────────────────────
      if (!object[std::string(keys::kind)])
      {
        errors.push_back({ ErrorCode::NotAnEmissionsConfig,
                           "Missing 'kind' field; emissions configs must declare 'kind: emissions'" });
        AppendFilePath(config_path_, errors);
        return std::unexpected(std::move(errors));
      }
      const std::string kind_val = object[std::string(keys::kind)].as<std::string>();
      if (kind_val != std::string(keys::kind_value))
      {
        const ErrorLocation loc{ object[std::string(keys::kind)].Mark().line,
                                 object[std::string(keys::kind)].Mark().column };
        errors.push_back({ ErrorCode::NotAnEmissionsConfig,
                           mc_fmt::format("{} error: Expected 'kind: emissions', found 'kind: {}'", loc, kind_val) });
        AppendFilePath(config_path_, errors);
        return std::unexpected(std::move(errors));
      }

      // ── version ───────────────────────────────────────────────────────────────
      if (!object[std::string(keys::version)])
      {
        errors.push_back({ ErrorCode::MissingVersionField, "Missing required 'version' field" });
        AppendFilePath(config_path_, errors);
        return std::unexpected(std::move(errors));
      }
      const Version version(object[std::string(keys::version)].as<std::string>());
      constexpr unsigned int SUPPORTED_MAJOR = 1;
      if (version.major != SUPPORTED_MAJOR)
      {
        const ErrorLocation loc{ object[std::string(keys::version)].Mark().line,
                                 object[std::string(keys::version)].Mark().column };
        errors.push_back({ ErrorCode::InvalidVersion,
                           mc_fmt::format(
                               "{} error: Unsupported emissions config version '{}'; only major version {} is supported",
                               loc,
                               version.to_string(),
                               SUPPORTED_MAJOR) });
        AppendFilePath(config_path_, errors);
        return std::unexpected(std::move(errors));
      }

      types::EmissionsConfig config;
      config.version = version;

      // ── name (optional) ───────────────────────────────────────────────────────
      if (object[std::string(keys::name)])
        config.name = object[std::string(keys::name)].as<std::string>();

      // ── data root (optional) ──────────────────────────────────────────────────
      if (object[std::string(keys::data_root)])
      {
        const std::string raw = object[std::string(keys::data_root)].as<std::string>();
        config.data_root = ExpandEnvVars(raw, errors);
        if (!errors.empty())
        {
          AppendFilePath(config_path_, errors);
          return std::unexpected(std::move(errors));
        }
      }

      // ── inventories (optional) ────────────────────────────────────────────────
      if (object[std::string(keys::inventories)])
      {
        const YAML::Node& inv_node = object[std::string(keys::inventories)];
        if (!inv_node.IsMap())
        {
          errors.push_back({ ErrorCode::InvalidType, "'inventories' must be a mapping" });
        }
        else
        {
          for (const auto& kv : inv_node)
          {
            const std::string inv_name = kv.first.as<std::string>();
            if (config.inventories.count(inv_name))
            {
              errors.push_back(
                  { ErrorCode::DuplicateInventoryDetected,
                    mc_fmt::format("Duplicate inventory name '{}'", inv_name) });
              continue;
            }
            const YAML::Node& entry = kv.second;
            types::Inventory inv;
            if (!entry[std::string(keys::directory)])
              errors.push_back(
                  { ErrorCode::RequiredKeyNotFound,
                    mc_fmt::format("Inventory '{}' is missing required key 'directory'", inv_name) });
            else
              inv.directory = entry[std::string(keys::directory)].as<std::string>();

            if (!entry[std::string(keys::file_pattern)])
              errors.push_back(
                  { ErrorCode::RequiredKeyNotFound,
                    mc_fmt::format("Inventory '{}' is missing required key 'file pattern'", inv_name) });
            else
              inv.file_pattern = entry[std::string(keys::file_pattern)].as<std::string>();

            if (!entry[std::string(keys::convention)])
              errors.push_back(
                  { ErrorCode::RequiredKeyNotFound,
                    mc_fmt::format("Inventory '{}' is missing required key 'convention'", inv_name) });
            else
              inv.convention = entry[std::string(keys::convention)].as<std::string>();

            config.inventories.emplace(inv_name, std::move(inv));
          }
        }
      }

      // ── species maps (optional) ───────────────────────────────────────────────
      if (object[std::string(keys::species_maps)])
      {
        const YAML::Node& sm_node = object[std::string(keys::species_maps)];
        if (!sm_node.IsMap())
        {
          errors.push_back({ ErrorCode::InvalidType, "'species maps' must be a mapping" });
        }
        else
        {
          for (const auto& kv : sm_node)
          {
            const std::string map_name = kv.first.as<std::string>();
            if (config.species_maps.count(map_name))
            {
              errors.push_back(
                  { ErrorCode::DuplicateSpeciesMapDetected,
                    mc_fmt::format("Duplicate species map name '{}'", map_name) });
              continue;
            }
            const YAML::Node& entry = kv.second;
            types::SpeciesMap smap;

            if (!entry[std::string(keys::mappings)])
            {
              errors.push_back(
                  { ErrorCode::RequiredKeyNotFound,
                    mc_fmt::format("Species map '{}' is missing required key 'mappings'", map_name) });
              config.species_maps.emplace(map_name, std::move(smap));
              continue;
            }

            // Accumulate scaling factors per inventory species to validate the ≤1.0 constraint.
            std::unordered_map<std::string, double> inv_species_scale_sum;

            for (const auto& mapping_node : entry[std::string(keys::mappings)])
            {
              types::SpeciesMapping m;
              if (!mapping_node[std::string(keys::inventory_species)])
              {
                errors.push_back(
                    { ErrorCode::RequiredKeyNotFound,
                      mc_fmt::format("A mapping in species map '{}' is missing 'inventory species'", map_name) });
                continue;
              }
              if (!mapping_node[std::string(keys::mechanism_species)])
              {
                errors.push_back(
                    { ErrorCode::RequiredKeyNotFound,
                      mc_fmt::format("A mapping in species map '{}' is missing 'mechanism species'", map_name) });
                continue;
              }
              m.inventory_species = mapping_node[std::string(keys::inventory_species)].as<std::string>();
              m.mechanism_species = mapping_node[std::string(keys::mechanism_species)].as<std::string>();
              if (mapping_node[std::string(keys::scaling_factor)])
                m.scaling_factor = mapping_node[std::string(keys::scaling_factor)].as<double>();

              inv_species_scale_sum[m.inventory_species] += m.scaling_factor;
              smap.mappings.push_back(std::move(m));
            }

            // Validate scaling factor sums ≤ 1.0
            for (const auto& [inv_sp, total] : inv_species_scale_sum)
            {
              if (total > 1.0 + 1e-9)
              {
                errors.push_back(
                    { ErrorCode::SpeciesMapScalingExceedsOne,
                      mc_fmt::format(
                          "Species map '{}': scaling factors for inventory species '{}' sum to {:.4f}, which exceeds 1.0",
                          map_name,
                          inv_sp,
                          total) });
              }
            }
            config.species_maps.emplace(map_name, std::move(smap));
          }
        }
      }

      // ── regridding (optional) ─────────────────────────────────────────────────
      if (object[std::string(keys::regridding)])
      {
        const YAML::Node& rg_node = object[std::string(keys::regridding)];
        if (rg_node[std::string(keys::type)])
        {
          const std::string rg_type = rg_node[std::string(keys::type)].as<std::string>();
          if (rg_type == std::string(keys::regridding_none))
          {
            config.regridding.type = types::RegriddingType::None;
          }
          else
          {
            const ErrorLocation loc{ rg_node[std::string(keys::type)].Mark().line,
                                     rg_node[std::string(keys::type)].Mark().column };
            errors.push_back(
                { ErrorCode::UnsupportedRegriddingType,
                  mc_fmt::format(
                      "{} error: Unsupported regridding type '{}'; only 'none' is supported in v1", loc, rg_type) });
          }
        }
      }

      // ── sources (optional) ────────────────────────────────────────────────────
      if (object[std::string(keys::sources)])
      {
        const YAML::Node& src_node = object[std::string(keys::sources)];
        if (!src_node.IsSequence())
        {
          errors.push_back({ ErrorCode::InvalidType, "'sources' must be a sequence" });
        }
        else
        {
          std::set<std::string> seen_names;
          std::set<std::pair<int, int>> seen_cat_hier;

          for (const auto& s : src_node)
          {
            types::SourceDescriptor src;

            // name (required)
            if (!s[std::string(keys::name)])
            {
              errors.push_back({ ErrorCode::RequiredKeyNotFound, "A source entry is missing required key 'name'" });
              continue;
            }
            src.name = s[std::string(keys::name)].as<std::string>();

            if (seen_names.count(src.name))
            {
              errors.push_back(
                  { ErrorCode::DuplicateSourceDetected,
                    mc_fmt::format("Duplicate source name '{}'", src.name) });
              continue;
            }
            seen_names.insert(src.name);

            // mode (required)
            if (!s[std::string(keys::mode)])
            {
              errors.push_back(
                  { ErrorCode::RequiredKeyNotFound,
                    mc_fmt::format("Source '{}' is missing required key 'mode'", src.name) });
            }
            else
            {
              const std::string mode_val = s[std::string(keys::mode)].as<std::string>();
              if (mode_val == std::string(keys::mode_online))
              {
                const ErrorLocation loc{ s[std::string(keys::mode)].Mark().line,
                                         s[std::string(keys::mode)].Mark().column };
                errors.push_back(
                    { ErrorCode::OnlineSourcesNotSupported,
                      mc_fmt::format(
                          "{} error: Source '{}' uses 'mode: online', which is not supported in v1",
                          loc,
                          src.name) });
              }
              else if (mode_val != std::string(keys::mode_offline))
              {
                errors.push_back(
                    { ErrorCode::UnknownType,
                      mc_fmt::format("Source '{}' has unknown mode '{}'; expected 'offline'", src.name, mode_val) });
              }
              // mode_offline is the only valid value; src.mode is already Offline by default
            }

            // type (required)
            if (!s[std::string(keys::source_type)])
            {
              errors.push_back(
                  { ErrorCode::RequiredKeyNotFound,
                    mc_fmt::format("Source '{}' is missing required key 'type'", src.name) });
            }
            else
            {
              const std::string type_val = s[std::string(keys::source_type)].as<std::string>();
              static const std::vector<std::string_view> valid_types = { keys::type_anthropogenic, keys::type_fire,
                                                                         keys::type_biogenic,      keys::type_dust,
                                                                         keys::type_sea_salt,      keys::type_lightning };
              bool found = false;
              for (const auto& vt : valid_types)
                if (type_val == std::string(vt))
                  found = true;
              if (!found)
                errors.push_back(
                    { ErrorCode::UnknownType,
                      mc_fmt::format("Source '{}' has unknown type '{}'", src.name, type_val) });
              else
                src.type = ParseSourceType(type_val);
            }

            // inventory (required)
            if (!s[std::string(keys::inventory)])
            {
              errors.push_back(
                  { ErrorCode::RequiredKeyNotFound,
                    mc_fmt::format("Source '{}' is missing required key 'inventory'", src.name) });
            }
            else
            {
              src.inventory = s[std::string(keys::inventory)].as<std::string>();
              if (!config.inventories.empty() && !config.inventories.count(src.inventory))
              {
                errors.push_back(
                    { ErrorCode::SourceRequiresUnknownInventory,
                      mc_fmt::format(
                          "Source '{}' references inventory '{}' which is not declared in 'inventories'",
                          src.name,
                          src.inventory) });
              }
            }

            // species map (required)
            if (!s[std::string(keys::species_map)])
            {
              errors.push_back(
                  { ErrorCode::RequiredKeyNotFound,
                    mc_fmt::format("Source '{}' is missing required key 'species map'", src.name) });
            }
            else
            {
              src.species_map = s[std::string(keys::species_map)].as<std::string>();
              if (!config.species_maps.empty() && !config.species_maps.count(src.species_map))
              {
                errors.push_back(
                    { ErrorCode::SourceRequiresUnknownSpeciesMap,
                      mc_fmt::format(
                          "Source '{}' references species map '{}' which is not declared in 'species maps'",
                          src.name,
                          src.species_map) });
              }
            }

            // temporal interpolation (optional)
            if (s[std::string(keys::temporal_interpolation)])
              src.temporal_interpolation =
                  ParseTemporalInterpolation(s[std::string(keys::temporal_interpolation)].as<std::string>());

            // vertical injection (optional)
            if (s[std::string(keys::vertical_injection)])
            {
              const std::string vi_val = s[std::string(keys::vertical_injection)].as<std::string>();
              if (vi_val == std::string(keys::inject_plume))
              {
                const ErrorLocation loc{ s[std::string(keys::vertical_injection)].Mark().line,
                                         s[std::string(keys::vertical_injection)].Mark().column };
                errors.push_back(
                    { ErrorCode::UnsupportedVerticalInjection,
                      mc_fmt::format(
                          "{} error: Source '{}' uses 'vertical injection: plume', which is not supported in v1",
                          loc,
                          src.name) });
              }
              else if (vi_val != std::string(keys::inject_surface))
              {
                errors.push_back(
                    { ErrorCode::UnknownType,
                      mc_fmt::format(
                          "Source '{}' has unknown vertical injection '{}'; expected 'surface'", src.name, vi_val) });
              }
            }

            // category, hierarchy, scaling factor, sector (all optional)
            if (s[std::string(keys::category)])
              src.category = s[std::string(keys::category)].as<int>();
            if (s[std::string(keys::hierarchy)])
              src.hierarchy = s[std::string(keys::hierarchy)].as<int>();
            if (s[std::string(keys::scaling_factor)])
              src.scaling_factor = s[std::string(keys::scaling_factor)].as<double>();
            if (s[std::string(keys::sector)])
              src.sector = s[std::string(keys::sector)].as<std::string>();

            // duplicate (category, hierarchy)
            const auto cat_hier = std::make_pair(src.category, src.hierarchy);
            if (seen_cat_hier.count(cat_hier))
            {
              errors.push_back(
                  { ErrorCode::DuplicateCategoryHierarchy,
                    mc_fmt::format(
                        "Source '{}' has duplicate (category: {}, hierarchy: {}) — each (category, hierarchy) pair must be unique",
                        src.name,
                        src.category,
                        src.hierarchy) });
            }
            else
            {
              seen_cat_hier.insert(cat_hier);
            }

            src.unknown_properties = GetUnknownProperties(s);
            config.sources.push_back(std::move(src));
          }
        }
      }

      if (!errors.empty())
      {
        AppendFilePath(config_path_, errors);
        return std::unexpected(std::move(errors));
      }

      return config;
    }
    catch (const std::exception& e)
    {
      const std::string where = config_path_.empty() ? "document" : "'" + config_path_ + "'";
      return std::unexpected(
          Errors{ { ErrorCode::UnexpectedError,
                    mc_fmt::format("Failed to parse emissions config {}: {}", where, e.what()) } });
    }
  }

  std::expected<types::EmissionsConfig, Errors> Parser::Parse(const std::filesystem::path& config_path)
  {
    config_path_ = config_path.string();

    if (!std::filesystem::exists(config_path))
    {
      return std::unexpected(Errors{ { ErrorCode::FileNotFound,
                                       mc_fmt::format("Configuration file '{}' does not exist.", config_path.string()) } });
    }

    YAML::Node object;
    try
    {
      object = YAML::LoadFile(config_path.string());
    }
    catch (const std::exception& e)
    {
      return std::unexpected(
          Errors{ { ErrorCode::UnexpectedError,
                    mc_fmt::format("Failed to parse '{}': {}", config_path.string(), e.what()) } });
    }

    return ValidateAndBuild(object);
  }

  std::expected<types::EmissionsConfig, Errors> Parser::Parse(const std::string& content)
  {
    config_path_ = "";

    YAML::Node object;
    try
    {
      object = YAML::Load(content);
    }
    catch (const std::exception& e)
    {
      return std::unexpected(
          Errors{ { ErrorCode::UnexpectedError,
                    mc_fmt::format("Failed to parse emissions document: {}", e.what()) } });
    }

    return ValidateAndBuild(object);
  }

  // ── Public free functions ──────────────────────────────────────────────────

  std::expected<types::EmissionsConfig, Errors> ParseEmissions(const std::filesystem::path& config_path)
  {
    return Parser{}.Parse(config_path);
  }

  std::expected<types::EmissionsConfig, Errors> ParseEmissions(const std::string& content)
  {
    return Parser{}.Parse(content);
  }

}  // namespace mechanism_configuration::emissions::v1

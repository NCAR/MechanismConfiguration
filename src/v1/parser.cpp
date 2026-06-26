// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/parser.hpp"

#include "detail/check_schema.hpp"
#include "detail/v1/keys.hpp"
#include "detail/v1/type_parsers.hpp"
#include "detail/v1/type_schema.hpp"
#include "detail/v1/utils.hpp"

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/mechanism.hpp>

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <optional>
#include <set>
#include <string_view>

namespace mechanism_configuration::v1
{
  namespace
  {
    // How a top-level section (species / phases / reactions) is expressed.
    enum class EntityFormat
    {
      FileList,  // { "files": [...] } — split across other files (v1.1+)
      Inline,    // [ ... ] — listed directly
      Invalid,   // present but neither of the above
    };

    EntityFormat GetEntityFormat(const YAML::Node& node)
    {
      if (node.IsMap() && node["files"])
        return EntityFormat::FileList;
      if (node.IsSequence())
        return EntityFormat::Inline;
      return EntityFormat::Invalid;
    }

    ErrorLocation LocationOf(const YAML::Node& node)
    {
      return ErrorLocation{ node.Mark().line, node.Mark().column };
    }

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
      if (s == std::string(keys::type_fire))      return types::SourceType::Fire;
      if (s == std::string(keys::type_biogenic))  return types::SourceType::Biogenic;
      if (s == std::string(keys::type_dust))      return types::SourceType::Dust;
      if (s == std::string(keys::type_sea_salt))  return types::SourceType::SeaSalt;
      if (s == std::string(keys::type_lightning)) return types::SourceType::Lightning;
      return types::SourceType::Anthropogenic;
    }

    types::TemporalInterpolation ParseTemporalInterpolation(const std::string& s)
    {
      if (s == std::string(keys::interp_nearest)) return types::TemporalInterpolation::Nearest;
      if (s == std::string(keys::interp_none))    return types::TemporalInterpolation::None;
      return types::TemporalInterpolation::Linear;
    }

    std::expected<types::EmissionsConfig, Errors> ParseEmissionsSection(const YAML::Node& node)
    {
      Errors errors;
      types::EmissionsConfig config;

      // inventories (optional)
      if (node[std::string(keys::inventories)])
      {
        const YAML::Node& inv_node = node[std::string(keys::inventories)];
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

      // species maps (optional)
      if (node[std::string(keys::species_maps)])
      {
        const YAML::Node& sm_node = node[std::string(keys::species_maps)];
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

            for (const auto& [inv_sp, total] : inv_species_scale_sum)
            {
              if (total > 1.0 + 1e-9)
                errors.push_back(
                    { ErrorCode::SpeciesMapScalingExceedsOne,
                      mc_fmt::format(
                          "Species map '{}': scaling factors for inventory species '{}' sum to {:.4f}, which exceeds 1.0",
                          map_name,
                          inv_sp,
                          total) });
            }
            config.species_maps.emplace(map_name, std::move(smap));
          }
        }
      }

      // regridding (optional)
      if (node[std::string(keys::regridding)])
      {
        const YAML::Node& rg_node = node[std::string(keys::regridding)];
        if (rg_node[std::string(keys::type)])
        {
          const std::string rg_type = rg_node[std::string(keys::type)].as<std::string>();
          if (rg_type == std::string(keys::regridding_none))
          {
            config.regridding.type = types::RegriddingType::None;
          }
          else
          {
            const ErrorLocation loc = LocationOf(rg_node[std::string(keys::type)]);
            errors.push_back(
                { ErrorCode::UnsupportedRegriddingType,
                  mc_fmt::format(
                      "{} error: Unsupported regridding type '{}'; only 'none' is supported in v1", loc, rg_type) });
          }
        }
      }

      // sources (optional)
      if (node[std::string(keys::sources)])
      {
        const YAML::Node& src_node = node[std::string(keys::sources)];
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
                const ErrorLocation loc = LocationOf(s[std::string(keys::mode)]);
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
            }

            if (!s[std::string(keys::type)])
            {
              errors.push_back(
                  { ErrorCode::RequiredKeyNotFound,
                    mc_fmt::format("Source '{}' is missing required key 'type'", src.name) });
            }
            else
            {
              const std::string type_val = s[std::string(keys::type)].as<std::string>();
              static const std::vector<std::string_view> valid_types = {
                keys::type_anthropogenic, keys::type_fire, keys::type_biogenic,
                keys::type_dust,          keys::type_sea_salt, keys::type_lightning
              };
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
                errors.push_back(
                    { ErrorCode::SourceRequiresUnknownInventory,
                      mc_fmt::format(
                          "Source '{}' references inventory '{}' which is not declared in 'inventories'",
                          src.name,
                          src.inventory) });
            }

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
                errors.push_back(
                    { ErrorCode::SourceRequiresUnknownSpeciesMap,
                      mc_fmt::format(
                          "Source '{}' references species map '{}' which is not declared in 'species maps'",
                          src.name,
                          src.species_map) });
            }

            if (s[std::string(keys::temporal_interpolation)])
              src.temporal_interpolation =
                  ParseTemporalInterpolation(s[std::string(keys::temporal_interpolation)].as<std::string>());

            if (s[std::string(keys::vertical_injection)])
            {
              const std::string vi_val = s[std::string(keys::vertical_injection)].as<std::string>();
              if (vi_val == std::string(keys::inject_plume))
              {
                const ErrorLocation loc = LocationOf(s[std::string(keys::vertical_injection)]);
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

            if (s[std::string(keys::category)])
              src.category = s[std::string(keys::category)].as<int>();
            if (s[std::string(keys::hierarchy)])
              src.hierarchy = s[std::string(keys::hierarchy)].as<int>();
            if (s[std::string(keys::scaling_factor)])
              src.scaling_factor = s[std::string(keys::scaling_factor)].as<double>();
            if (s[std::string(keys::sector)])
              src.sector = s[std::string(keys::sector)].as<std::string>();

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
        return std::unexpected(std::move(errors));
      return config;
    }

    // Appends each component under `key` (reactant- or product-like) as a located reference.
    void CollectComponents(const YAML::Node& reaction, std::string_view key, std::vector<semantics::NamedRef>& out)
    {
      const std::string k(key);
      if (!reaction[k])
        return;
      for (const auto& item : AsSequence(reaction[k]))
        out.push_back({ GetReactionComponentName(item), LocationOf(item) });
    }
  }  // namespace

  semantics::Input BuildSemanticInput(const YAML::Node& object)
  {
    semantics::Input input;

    if (object[std::string(keys::species)])
      for (const auto& s : object[std::string(keys::species)])
        input.species.push_back({ GetReactionComponentName(s), LocationOf(s) });

    if (object[std::string(keys::phases)])
      for (const auto& phase : object[std::string(keys::phases)])
      {
        semantics::PhaseRef pr;
        pr.name = phase[std::string(keys::name)].as<std::string>();
        pr.location = LocationOf(phase);
        if (phase[std::string(keys::species)])
          for (const auto& ps : phase[std::string(keys::species)])
            pr.species.push_back({ GetReactionComponentName(ps), LocationOf(ps) });
        input.phases.push_back(std::move(pr));
      }

    if (object[std::string(keys::reactions)])
      for (const auto& reaction : object[std::string(keys::reactions)])
      {
        semantics::ReactionRef rr;
        if (reaction[std::string(keys::type)])
          rr.type = reaction[std::string(keys::type)].as<std::string>();
        if (reaction[std::string(keys::gas_phase)])
        {
          rr.phase = reaction[std::string(keys::gas_phase)].as<std::string>();
          rr.location = LocationOf(reaction[std::string(keys::gas_phase)]);
        }
        // Reactant-like keys (must be in the reaction's phase).
        CollectComponents(reaction, keys::reactants, rr.reactants);
        CollectComponents(reaction, keys::gas_phase_species, rr.reactants);
        // Product-like keys (may reference any phase).
        CollectComponents(reaction, keys::products, rr.products);
        CollectComponents(reaction, keys::alkoxy_products, rr.products);
        CollectComponents(reaction, keys::nitrate_products, rr.products);
        CollectComponents(reaction, keys::gas_phase_products, rr.products);
        input.reactions.push_back(std::move(rr));
      }

    return input;
  }

  std::expected<YAML::Node, Errors> Parser::ResolveFileConfig(const std::filesystem::path& config_path)
  {
    if (!std::filesystem::exists(config_path) || !std::filesystem::is_regular_file(config_path))
    {
      return std::unexpected(Errors{
          { ErrorCode::FileNotFound,
            mc_fmt::format("Configuration file '{}' does not exist or is not a regular file.", config_path.string()) } });
    }

    SetConfigPath(config_path.string());

    YAML::Node object;
    try
    {
      object = YAML::LoadFile(config_path.string());
    }
    catch (const std::exception& e)
    {
      return std::unexpected(Errors{
          { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse '{}': {}", config_path.string(), e.what()) } });
    }

    Errors errors;
    const std::filesystem::path base_dir = config_path.parent_path();
    const Version version = object[keys::version] ? Version(object[keys::version].as<std::string>()) : Version();

    YAML::Node combined;
    if (object[keys::version])
      combined[std::string(keys::version)] = object[keys::version];
    if (object[keys::name])
      combined[std::string(keys::name)] = object[keys::name];

    // Loads and concatenates every file referenced under `<entity>.files`.
    auto load_files = [&](std::string_view entity) -> YAML::Node
    {
      YAML::Node merged(YAML::NodeType::Sequence);
      for (const auto& file_node : object[std::string(entity)]["files"])
      {
        const std::filesystem::path file_path = base_dir / file_node.as<std::string>();
        if (!std::filesystem::exists(file_path))
        {
          errors.push_back({ ErrorCode::FileNotFound, "File not found: " + file_path.string() });
          continue;
        }
        try
        {
          YAML::Node loaded = YAML::LoadFile(file_path.string());
          for (const auto& item : loaded)
            merged.push_back(item);
        }
        catch (const std::exception& e)
        {
          errors.push_back({ ErrorCode::UnexpectedError, "Failed to parse file: " + file_path.string() + ": " + e.what() });
        }
      }
      return merged;
    };

    // Resolves one section into the combined node. Missing sections are left out so the
    // normal schema validation reports them; malformed sections are flagged here.
    auto resolve_section = [&](std::string_view entity)
    {
      const std::string key(entity);
      if (!object[key])
        return;

      switch (GetEntityFormat(object[key]))
      {
        case EntityFormat::Inline: combined[key] = object[key]; break;
        case EntityFormat::FileList: combined[key] = load_files(entity); break;
        case EntityFormat::Invalid:
          if (object[key].IsMap())
            errors.push_back({ ErrorCode::RequiredKeyNotFound, "Missing 'files' key in '" + key + "' section." });
          else
            errors.push_back({ ErrorCode::InvalidType, "'" + key + "' must be a file-list object or an inline array." });
          break;
      }
    };
    // A file-list layout requires minor version >= 1; check before loading any files.
    const bool uses_filelist = (object[std::string(keys::species)] &&
                                GetEntityFormat(object[std::string(keys::species)]) == EntityFormat::FileList) ||
                               (object[std::string(keys::phases)] &&
                                GetEntityFormat(object[std::string(keys::phases)]) == EntityFormat::FileList) ||
                               (object[std::string(keys::reactions)] &&
                                GetEntityFormat(object[std::string(keys::reactions)]) == EntityFormat::FileList);
    if (uses_filelist && version.minor < 1)
    {
      errors.push_back({ ErrorCode::InvalidVersion,
                         "File-list format requires minor version >= 1, got " + std::to_string(version.minor) + "." });
      AppendFilePath(config_path_, errors);
      return std::unexpected(std::move(errors));
    }

    resolve_section(keys::species);
    resolve_section(keys::phases);
    resolve_section(keys::reactions);

    if (!errors.empty())
    {
      AppendFilePath(config_path_, errors);
      return std::unexpected(std::move(errors));
    }

    return combined;
  }

  Errors Parser::CheckSchema(const YAML::Node& object)
  {
    Errors errors;

    std::vector<std::string_view> required_keys = { keys::version, keys::species, keys::phases, keys::reactions };
    std::vector<std::string_view> optional_keys = { keys::name, keys::emissions };

    // Return early if the required keys are not found
    auto schema_errors = mechanism_configuration::CheckSchema(object, required_keys, optional_keys);
    if (!schema_errors.empty())
    {
      AppendFilePath(config_path_, schema_errors);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      return errors;
    }

    constexpr unsigned int MAJOR_VERSION = 1;
    Version version = Version(object[keys::version].as<std::string>());
    if (version.major != MAJOR_VERSION)
    {
      ErrorLocation error_location{ object[keys::version].Mark().line, object[keys::version].Mark().column };

      std::string message = mc_fmt::format(
          "{} error: The version must be '{}' but the invalid version number '{}' found.",
          error_location,
          MAJOR_VERSION,
          version.major);
      errors.push_back({ ErrorCode::InvalidVersion, config_path_ + ":" + message });
    }

    schema_errors = CheckSpeciesSchema(object[keys::species]);
    if (!schema_errors.empty())
    {
      AppendFilePath(config_path_, schema_errors);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      return errors;
    }

    auto parsed_species = ParseSpecies(object[keys::species]);

    schema_errors = CheckPhasesSchema(object[keys::phases], parsed_species);
    if (!schema_errors.empty())
    {
      AppendFilePath(config_path_, schema_errors);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      return errors;
    }

    auto parsed_phases = ParsePhases(object[keys::phases]);

    schema_errors = CheckReactionsSchema(object[keys::reactions], parsed_species, parsed_phases);
    if (!schema_errors.empty())
    {
      AppendFilePath(config_path_, schema_errors);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      return errors;
    }

    return errors;
  }

  std::expected<Mechanism, Errors> Parser::Parse(const std::filesystem::path& config_path)
  {
    // ResolveFileConfig sets config_path_ so errors carry the file path.
    auto object = ResolveFileConfig(config_path);
    if (!object)
    {
      return std::unexpected(std::move(object.error()));
    }
    return ValidateAndBuild(*object);
  }

  std::expected<Mechanism, Errors> Parser::Parse(const std::string& content)
  {
    SetDefaultConfigPath();  // no file backing this document
    YAML::Node object;
    try
    {
      object = YAML::Load(content);
    }
    catch (const std::exception& e)
    {
      return std::unexpected(
          Errors{ { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse document: {}", e.what()) } });
    }
    return ValidateAndBuild(object);
  }

  std::expected<Mechanism, Errors> Parser::Parse(const YAML::Node& object)
  {
    SetDefaultConfigPath();  // no file backing this node
    return ValidateAndBuild(object);
  }

  std::expected<Mechanism, Errors> Parser::ValidateAndBuild(const YAML::Node& object)
  {
    try
    {
      // 1) Structural (schema) validation.
      Errors errors = CheckSchema(object);

      // 2) Semantic validation — needs a structurally-valid document, so only run it when
      //    the structure is clean. Located via BuildSemanticInput so errors carry line:col.
      if (errors.empty())
      {
        auto semantic_errors = ValidateSemantics(BuildSemanticInput(object));
        AppendFilePath(config_path_, semantic_errors);
        errors.insert(errors.end(), semantic_errors.begin(), semantic_errors.end());
      }

      // 3) Parse optional emissions section.
      std::optional<types::EmissionsConfig> emissions;
      if (errors.empty() && object[std::string(keys::emissions)])
      {
        auto em = ParseEmissionsSection(object[std::string(keys::emissions)]);
        if (!em)
        {
          AppendFilePath(config_path_, em.error());
          errors.insert(errors.end(), em.error().begin(), em.error().end());
        }
        else
        {
          emissions = std::move(*em);
        }
      }

      if (!errors.empty())
      {
        return std::unexpected(std::move(errors));
      }

      // 4) Build the Mechanism (only reached when fully valid).
      return Build(object, std::move(emissions));
    }
    catch (const std::exception& e)
    {
      const std::string where = config_path_.empty() ? "document" : "'" + config_path_ + "'";
      return std::unexpected(
          Errors{ { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse {}: {}", where, e.what()) } });
    }
  }

  Mechanism Parser::Build(const YAML::Node& object, std::optional<types::EmissionsConfig> emissions)
  {
    Mechanism mechanism;

    mechanism.version = Version(object[keys::version].as<std::string>());
    mechanism.species = ParseSpecies(object[keys::species]);
    mechanism.phases = ParsePhases(object[keys::phases]);
    mechanism.reactions = ParseReactions(object[keys::reactions]);
    mechanism.emissions = std::move(emissions);

    if (object[keys::name])
    {
      mechanism.name = object[keys::name].as<std::string>();
    }

    return mechanism;
  }

}  // namespace mechanism_configuration::v1

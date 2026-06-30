// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/parser.hpp"

#include "detail/check_schema.hpp"
#include "detail/error_format.hpp"
#include "detail/v1/keys.hpp"
#include "detail/v1/type_parsers.hpp"
#include "detail/v1/type_schema.hpp"
#include "detail/v1/utils.hpp"

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/mechanism.hpp>

#include <yaml-cpp/yaml.h>

#include <filesystem>
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
    std::vector<std::string_view> optional_keys = { keys::name };

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

      if (!errors.empty())
      {
        return std::unexpected(std::move(errors));
      }

      // 3) Build the Mechanism (only reached when fully valid).
      return Build(object);
    }
    catch (const std::exception& e)
    {
      const std::string where = config_path_.empty() ? "document" : "'" + config_path_ + "'";
      return std::unexpected(
          Errors{ { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse {}: {}", where, e.what()) } });
    }
  }

  Mechanism Parser::Build(const YAML::Node& object)
  {
    Mechanism mechanism;

    mechanism.version = Version(object[keys::version].as<std::string>());
    mechanism.species = ParseSpecies(object[keys::species]);
    mechanism.phases = ParsePhases(object[keys::phases]);
    mechanism.reactions = ParseReactions(object[keys::reactions]);

    if (object[keys::name])
    {
      mechanism.name = object[keys::name].as<std::string>();
    }

    return mechanism;
  }

}  // namespace mechanism_configuration::v1

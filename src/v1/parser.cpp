// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/parser.hpp"
#include "detail/v1/type_parsers.hpp"
#include "detail/v1/type_schema.hpp"
#include "detail/v1/utils.hpp"
#include "detail/check_schema.hpp"
#include "detail/validation_keys.hpp"
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
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

    if (object[std::string(validation::species)])
      for (const auto& s : object[std::string(validation::species)])
        input.species.push_back({ GetReactionComponentName(s), LocationOf(s) });

    if (object[std::string(validation::phases)])
      for (const auto& phase : object[std::string(validation::phases)])
      {
        semantics::PhaseRef pr;
        pr.name = phase[std::string(validation::name)].as<std::string>();
        pr.location = LocationOf(phase);
        if (phase[std::string(validation::species)])
          for (const auto& ps : phase[std::string(validation::species)])
            pr.species.push_back({ GetReactionComponentName(ps), LocationOf(ps) });
        input.phases.push_back(std::move(pr));
      }

    if (object[std::string(validation::reactions)])
      for (const auto& reaction : object[std::string(validation::reactions)])
      {
        semantics::ReactionRef rr;
        if (reaction[std::string(validation::type)])
          rr.type = reaction[std::string(validation::type)].as<std::string>();
        if (reaction[std::string(validation::gas_phase)])
        {
          rr.phase = reaction[std::string(validation::gas_phase)].as<std::string>();
          rr.phase_location = LocationOf(reaction[std::string(validation::gas_phase)]);
        }
        // Reactant-like keys (must be in the reaction's phase).
        CollectComponents(reaction, validation::reactants, rr.reactants);
        CollectComponents(reaction, validation::gas_phase_species, rr.reactants);
        // Product-like keys (may reference any phase).
        CollectComponents(reaction, validation::products, rr.products);
        CollectComponents(reaction, validation::alkoxy_products, rr.products);
        CollectComponents(reaction, validation::nitrate_products, rr.products);
        CollectComponents(reaction, validation::gas_phase_products, rr.products);
        input.reactions.push_back(std::move(rr));
      }

    return input;
  }

  YAML::Node Parser::FileToYaml(const std::filesystem::path& config_path)
  {
    if (!std::filesystem::exists(config_path) || !std::filesystem::is_regular_file(config_path))
    {
      throw std::runtime_error(
          mc_fmt::format("Configuration file '{}' does not exist or is not a regular file.", config_path.string()));
    }

    SetConfigPath(config_path.string());

    try
    {
      return YAML::LoadFile(config_path.string());
    }
    catch (const YAML::Exception& e)
    {
      throw std::runtime_error(mc_fmt::format("Failed to parse '{}': {}", config_path.string(), e.what()));
    }
  }

  std::expected<YAML::Node, Errors> Parser::ResolveFileConfig(const std::filesystem::path& config_path)
  {
    if (!std::filesystem::exists(config_path) || !std::filesystem::is_regular_file(config_path))
    {
      return std::unexpected(
          Errors{ { ErrorCode::FileNotFound,
                    mc_fmt::format(
                        "Configuration file '{}' does not exist or is not a regular file.", config_path.string()) } });
    }

    SetConfigPath(config_path.string());

    YAML::Node object;
    try
    {
      object = YAML::LoadFile(config_path.string());
    }
    catch (const std::exception& e)
    {
      return std::unexpected(
          Errors{
              { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse '{}': {}", config_path.string(), e.what()) } });
    }

    Errors errors;
    const std::filesystem::path base_dir = config_path.parent_path();
    const Version version = object[validation::version] ? Version(object[validation::version].as<std::string>()) : Version();

    YAML::Node combined;
    if (object[validation::version])
      combined[std::string(validation::version)] = object[validation::version];
    if (object[validation::name])
      combined[std::string(validation::name)] = object[validation::name];

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
    const bool uses_filelist = (object[std::string(validation::species)] &&
                                GetEntityFormat(object[std::string(validation::species)]) == EntityFormat::FileList) ||
                               (object[std::string(validation::phases)] &&
                                GetEntityFormat(object[std::string(validation::phases)]) == EntityFormat::FileList) ||
                               (object[std::string(validation::reactions)] &&
                                GetEntityFormat(object[std::string(validation::reactions)]) == EntityFormat::FileList);
    if (uses_filelist && version.minor < 1)
    {
      errors.push_back(
          { ErrorCode::InvalidVersion,
            "File-list format requires minor version >= 1, got " + std::to_string(version.minor) + "." });
      AppendFilePath(config_path_, errors);
      return std::unexpected(std::move(errors));
    }

    resolve_section(validation::species);
    resolve_section(validation::phases);
    resolve_section(validation::reactions);

    if (!errors.empty())
    {
      AppendFilePath(config_path_, errors);
      return std::unexpected(std::move(errors));
    }

    return combined;
  }

  Errors Parser::CheckSchema(const YAML::Node& object, bool read_from_config_file)
  {
    if (!read_from_config_file)
    {
      SetDefaultConfigPath();
    }

    Errors errors;

    std::vector<std::string_view> required_keys = {
      validation::version, validation::species, validation::phases, validation::reactions
    };
    std::vector<std::string_view> optional_keys = { validation::name };

    // Return early if the required keys are not found
    auto schema_errors = mechanism_configuration::CheckSchema(object, required_keys, optional_keys);
    if (!schema_errors.empty())
    {
      AppendFilePath(config_path_, schema_errors);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      return errors;
    }

    constexpr unsigned int MAJOR_VERSION = 1;
    Version version = Version(object[validation::version].as<std::string>());
    if (version.major != MAJOR_VERSION)
    {
      ErrorLocation error_location{ object[validation::version].Mark().line, object[validation::version].Mark().column };

      std::string message = mc_fmt::format(
          "{} error: The version must be '{}' but the invalid version number '{}' found.",
          error_location,
          MAJOR_VERSION,
          version.major);
      errors.push_back({ ErrorCode::InvalidVersion, config_path_ + ":" + message });
    }

    schema_errors = CheckSpeciesSchema(object[validation::species]);
    if (!schema_errors.empty())
    {
      AppendFilePath(config_path_, schema_errors);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      return errors;
    }

    auto parsed_species = ParseSpecies(object[validation::species]);

    schema_errors = CheckPhasesSchema(object[validation::phases], parsed_species);
    if (!schema_errors.empty())
    {
      AppendFilePath(config_path_, schema_errors);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      return errors;
    }

    auto parsed_phases = ParsePhases(object[validation::phases]);

    schema_errors = CheckReactionsSchema(object[validation::reactions], parsed_species, parsed_phases);
    if (!schema_errors.empty())
    {
      AppendFilePath(config_path_, schema_errors);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      return errors;
    }

    // Structure is valid; run the version-neutral semantic checks over a located intermediate
    // so errors carry line:col. These rules live only in ValidateSemantics.
    auto semantic_errors = ValidateSemantics(BuildSemanticInput(object));
    if (!semantic_errors.empty())
    {
      AppendFilePath(config_path_, semantic_errors);
      errors.insert(errors.end(), semantic_errors.begin(), semantic_errors.end());
    }

    return errors;
  }

  std::expected<Mechanism, Errors> Parser::Parse(const YAML::Node& object, bool read_from_config_file)
  {
    if (Errors errors = CheckSchema(object, read_from_config_file); !errors.empty())
    {
      return std::unexpected(std::move(errors));
    }
    return Build(object);
  }

  Mechanism Parser::Build(const YAML::Node& object)
  {
    Mechanism mechanism;

    mechanism.version = Version(object[validation::version].as<std::string>());
    mechanism.species = ParseSpecies(object[validation::species]);
    mechanism.phases = ParsePhases(object[validation::phases]);
    mechanism.reactions = ParseReactions(object[validation::reactions]);

    if (object[validation::name])
    {
      mechanism.name = object[validation::name].as<std::string>();
    }

    return mechanism;
  }

}  // namespace mechanism_configuration::v1

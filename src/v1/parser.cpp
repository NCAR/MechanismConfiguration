// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/v1/mechanism_parsers.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/v1/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration::v1
{
  Parser::EntityFormat Parser::GetEntityFormat(const YAML::Node& node)
  {
    if (node.IsMap() && node["files"])
      return EntityFormat::FileList;
    if (node.IsSequence())
      return EntityFormat::Inline;
    return EntityFormat::Invalid;
  }

  std::expected<Mechanism, Errors> Parser::ParseFromString(const std::string& content)
  {
    std::expected<Mechanism, Errors> result;
    try
    {
      YAML::Node object = YAML::Load(content);

      std::vector<std::string_view> required_keys = {
        validation::version, validation::species, validation::phases, validation::reactions
      };
      std::vector<std::string_view> optional_keys = { validation::name };

      auto validate_errors = ValidateSchema(object, required_keys, optional_keys);
      if (!validate_errors.empty())
      {
        result = std::unexpected(validate_errors);
      }
      else
      {
        result = ParseFromNode(object);
      }
    }
    catch (const std::exception& e)
    {
      std::string msg = "Failed to parse content as YAML: " + std::string(e.what());
      msg += "\nContent:\n" + content;
      result = std::unexpected(Errors{ { ErrorCode::UnexpectedError, msg } });
    }

    return result;
  }

  std::expected<Mechanism, Errors> Parser::Parse(const std::filesystem::path& config_path)
  {
    std::expected<Mechanism, Errors> result;
    Errors errors;

    auto prepend_path = [&](Errors& errs)
    {
      const std::string prefix = config_path.string() + ":";
      for (auto& error : errs)
        error.second = prefix + error.second;
    };

    if (!std::filesystem::exists(config_path) || !std::filesystem::is_regular_file(config_path))
    {
      errors.push_back({ ErrorCode::FileNotFound, "File not found or is a directory" });
    }
    else
    {
      try
      {
        YAML::Node object = YAML::LoadFile(config_path.string());

        std::vector<std::string_view> mechanism_required_keys = {
          validation::version, validation::species, validation::phases, validation::reactions
        };
        std::vector<std::string_view> mechanism_optional_keys = { validation::name };

        auto validate_errors = ValidateSchema(object, mechanism_required_keys, mechanism_optional_keys);

        if (!validate_errors.empty())
        {
          errors = std::move(validate_errors);
        }
        else
        {
          Version version = Version(object[validation::version].as<std::string>());
          if (version.major != 1)
          {
            errors.push_back(
                { ErrorCode::InvalidVersion,
                  "Unsupported version '" + object[validation::version].as<std::string>() +
                      "'. Expected major version 1." });
          }
          else
          {
            EntityFormat spc_format = GetEntityFormat(object[validation::species]);
            EntityFormat phs_format = GetEntityFormat(object[validation::phases]);
            EntityFormat rxn_format = GetEntityFormat(object[validation::reactions]);

            auto check_invalid = [&](std::string_view entity_view, EntityFormat fmt)
            {
              const std::string entity(entity_view);
              if (fmt == EntityFormat::Invalid)
              {
                if (object[entity] && object[entity].IsMap())
                  errors.push_back({ ErrorCode::RequiredKeyNotFound, "Missing 'files' key in '" + entity + "' section." });
                else
                  errors.push_back(
                      { ErrorCode::InvalidType, "'" + entity + "' must be a file-list object or an inline array." });
              }
            };
            check_invalid(validation::species, spc_format);
            check_invalid(validation::phases, phs_format);
            check_invalid(validation::reactions, rxn_format);

            if (errors.empty())
            {
              bool any_filelist = spc_format == EntityFormat::FileList || phs_format == EntityFormat::FileList ||
                                  rxn_format == EntityFormat::FileList;
              if (any_filelist && version.minor < 1)
              {
                errors.push_back(
                    { ErrorCode::InvalidVersion,
                      "File-list format requires minor version >= 1, got " + std::to_string(version.minor) + "." });
              }
              else
              {
                auto parsed = ParseFromFileConfig(object, config_path, spc_format, phs_format, rxn_format);
                if (parsed)
                  result = std::move(*parsed);
                else
                  errors = std::move(parsed.error());
              }
            }
          }
        }
      }
      catch (const std::exception& e)
      {
        errors.push_back(
            { ErrorCode::UnexpectedError, "Failed to parse '" + config_path.string() + "': " + std::string(e.what()) });
      }
    }

    if (!errors.empty())
    {
      prepend_path(errors);
      result = std::unexpected(std::move(errors));
    }

    return result;
  }

  std::expected<Mechanism, Errors> Parser::ParseFromNode(const YAML::Node& object)
  {
    Errors errors;
    Mechanism mechanism;

    mechanism.version = Version(object[validation::version].as<std::string>());

    if (object[validation::name])
    {
      mechanism.name = object[validation::name].as<std::string>();
    }

    auto species_parsing = ParseSpecies(object[validation::species]);
    errors.insert(errors.end(), species_parsing.first.begin(), species_parsing.first.end());
    mechanism.species = species_parsing.second;

    auto phases_parsing = ParsePhases(object[validation::phases], species_parsing.second);
    errors.insert(errors.end(), phases_parsing.first.begin(), phases_parsing.first.end());
    mechanism.phases = phases_parsing.second;

    auto reactions_parsing = ParseReactions(object[validation::reactions], species_parsing.second, phases_parsing.second);
    errors.insert(errors.end(), reactions_parsing.first.begin(), reactions_parsing.first.end());
    mechanism.reactions = reactions_parsing.second;

    if (!errors.empty())
      return std::unexpected(std::move(errors));

    return mechanism;
  }

  std::expected<Mechanism, Errors> Parser::ParseFromFileConfig(
      const YAML::Node& object,
      const std::filesystem::path& config_path,
      EntityFormat spc_format,
      EntityFormat phs_format,
      EntityFormat rxn_format)
  {
    Errors errors;

    std::filesystem::path base_dir = config_path.parent_path();

    auto load_files = [&](std::string_view entity) -> std::pair<Errors, YAML::Node>
    {
      Errors errors;
      YAML::Node merged = YAML::Node(YAML::NodeType::Sequence);

      for (const auto& file_node : object[entity]["files"])
      {
        std::filesystem::path file_path = base_dir / file_node.as<std::string>();
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
      return { errors, merged };
    };

    auto resolve_section = [&](std::string_view entity, EntityFormat fmt) -> std::pair<Errors, YAML::Node>
    {
      if (fmt == EntityFormat::Inline)
        return { {}, object[entity] };
      return load_files(entity);
    };

    YAML::Node combined;
    combined[validation::version] = object[validation::version];
    if (object[validation::name])
      combined[validation::name] = object[validation::name];

    auto [species_errors, species_node] = resolve_section(validation::species, spc_format);
    errors.insert(errors.end(), species_errors.begin(), species_errors.end());
    combined[validation::species] = species_node;

    auto [phases_errors, phases_node] = resolve_section(validation::phases, phs_format);
    errors.insert(errors.end(), phases_errors.begin(), phases_errors.end());
    combined[validation::phases] = phases_node;

    auto [reactions_errors, reactions_node] = resolve_section(validation::reactions, rxn_format);
    errors.insert(errors.end(), reactions_errors.begin(), reactions_errors.end());
    combined[validation::reactions] = reactions_node;

    auto parsed = ParseFromNode(combined);
    if (!parsed)
      errors.insert(errors.end(), parsed.error().begin(), parsed.error().end());

    if (!errors.empty())
      return std::unexpected(std::move(errors));

    return parsed;
  }
}  // namespace mechanism_configuration::v1
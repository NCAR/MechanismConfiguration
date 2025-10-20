// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/utils.hpp>
#include <mechanism_configuration/development/validation.hpp>
#include <mechanism_configuration/error_location.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <format>

namespace mechanism_configuration
{
  namespace development
  {

    void AppendFilePath(const std::filesystem::path& config_path, Errors& errors)
    {
      for (auto& error : errors)
      {
        error.second = config_path.string() + ":" + error.second;
      }
    }

    std::unordered_map<std::string, std::string> GetComments(const YAML::Node& object)
    {
      std::unordered_map<std::string, std::string> unknown_properties;
      const std::string comment_start = "__";

      for (const auto& key : object)
      {
        std::string key_str = key.first.as<std::string>();

        // Check if the key starts with the comment prefix
        if (key_str.compare(0, comment_start.size(), comment_start) == 0)
        {
          // Check if the value is a YAML node
          if (key.second.IsScalar())
          {
            unknown_properties[key_str] = key.second.as<std::string>();
          }
          else
          {
            std::stringstream ss;
            ss << key.second;
            unknown_properties[key_str] = ss.str();
          }
        }
      }

      // Return the map of extracted comments
      return unknown_properties;
    }

    std::vector<std::string> GetSpeciesNames(const std::vector<types::PhaseSpecies>& phase_species)
    {
      std::vector<std::string> names;
      names.reserve(phase_species.size());
      for (const auto& species : phase_species)
      {
        names.push_back(species.name);
      }
      return names;
    }

    void ReportUnknownSpecies(
        const YAML::Node& object,
        const std::vector<NodeInfo>& unknown_species,
        Errors& errors,
        const ConfigParseStatus& parser_status)
    {
      if (unknown_species.empty())
        return;

      for (const auto& [name, node] : unknown_species)
      {
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string message = std::format(
            "{} error: Unknown species name '{}' found in '{}'.",
            error_location,
            name,
            object[validation::type].as<std::string>());

        errors.push_back({ parser_status, message });
      }
    }

    void CheckPhaseExists(
        const YAML::Node& object,
        const std::string& phase_key,
        const std::vector<types::Phase>& existing_phases,
        Errors& errors,
        const ConfigParseStatus& parser_status)
    {
      if (!object[phase_key])
        return;

      const auto& phase_node = object[phase_key];
      std::string phase_name = phase_node.as<std::string>();

      auto it = std::find_if(
          existing_phases.begin(),
          existing_phases.end(),
          [&phase_name](const auto& phase) { return phase.name == phase_name; });

      if (it == existing_phases.end())
      {
        ErrorLocation error_location{ phase_node.Mark().line, phase_node.Mark().column };

        std::string message = std::format(
            "{} error: Unknown phase name '{}' found in '{}'.",
            error_location,
            phase_name,
            object[validation::type].as<std::string>());

        errors.push_back({ parser_status, message });
      }
    }

  }  // namespace development
}  // namespace mechanism_configuration

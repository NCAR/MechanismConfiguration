// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/development/model_types.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/types.hpp>
#include <mechanism_configuration/development/validation.hpp>
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/parse_status.hpp>

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mechanism_configuration
{
  namespace development
  {
    struct NodeInfo
    {
      std::string name;
      YAML::Node nodes;
    };

    struct DuplicateEntryInfo
    {
      std::string name;
      std::vector<YAML::Node> nodes;
    };

    void AppendFilePath(const std::filesystem::path& config_path, Errors& errors);

    std::unordered_map<std::string, std::string> GetComments(const YAML::Node& object);

    /// @brief Extract species names from a vector of PhaseSpecies
    std::vector<std::string> GetSpeciesNames(const std::vector<types::PhaseSpecies>& phase_species);

    void ReportUnknownSpecies(
      const YAML::Node& object,
      const std::vector<NodeInfo>& unknown_species,
      Errors& errors,
      const ConfigParseStatus& parser_status = ConfigParseStatus::UnknownSpecies);

    void CheckPhaseExists(
      const YAML::Node& object,
      const std::string& phase_key,
      const std::vector<types::Phase>& existing_phases,
      Errors& errors,
      const ConfigParseStatus& parser_status = ConfigParseStatus::UnknownPhase);

    template<typename T>
    std::vector<DuplicateEntryInfo> FindDuplicateObjectsByName(const std::vector<std::pair<T, YAML::Node>>& collection)
    {
      std::unordered_map<std::string, std::vector<YAML::Node>> name_to_nodes;

      if constexpr (std::is_same_v<T, std::string>)
      {
        for (const auto& [elem, node] : collection)
        {
          name_to_nodes[elem].push_back(node);
        }
      }
      else
      {
        for (const auto& [elem, node] : collection)
        {
          name_to_nodes[elem.name].push_back(node);
        }
      }

      std::vector<DuplicateEntryInfo> duplicates;

      for (const auto& [name, nodes] : name_to_nodes)
      {
        if (nodes.size() > 1)
        {
          duplicates.push_back({ name, nodes });
        }
      }

      return duplicates;
    }

    template<typename ExistingType, typename RequestedType>
    std::vector<NodeInfo> FindUnknownObjectsByName(
        const std::vector<ExistingType>& existing_objects,
        std::vector<std::pair<RequestedType, YAML::Node>>& requested_objects)
    {
      std::unordered_set<std::string> existing_names;

      if constexpr (std::is_same_v<ExistingType, std::string>)
      {
        for (const auto& name : existing_objects)
        {
          existing_names.insert(name);
        }
      }
      else
      {
        for (const auto& object : existing_objects)
        {
          existing_names.insert(object.name);
        }
      }

      std::vector<NodeInfo> unknowns;

      if constexpr (std::is_same_v<RequestedType, std::string>)
      {
        for (const auto& [name, node] : requested_objects)
        {
          if (existing_names.find(name) == existing_names.end())
          {
            unknowns.emplace_back(name, node);
          }
        }
      }
      else
      {
        for (const auto& [elem, node] : requested_objects)
        {
          const auto& name = elem.name;
          if (existing_names.find(name) == existing_names.end())
          {
            unknowns.emplace_back(name, node);
          }
        }
      }

      return unknowns;
    }

    // TODO (In Progress): This function will be removed and replaced by the one above
    //                     once parsing and validation for all the configurations are fully decoupled.
    template<typename SpeciesType>
    std::vector<std::string> FindUnknownSpecies(
        const std::vector<std::string>& requested_species,
        const std::vector<SpeciesType>& existing_species)
    {
      std::unordered_set<std::string> existing_names;

      if constexpr (std::is_same_v<SpeciesType, std::string>)
      {
        for (const auto& species : existing_species)
        {
          existing_names.insert(species);
        }
      }
      else
      {
        for (const auto& species : existing_species)
        {
          existing_names.insert(species.name);
        }
      }

      std::vector<std::string> unknown_species;
      for (const auto& name : requested_species)
      {
        if (existing_names.find(name) == existing_names.end())
        {
          unknown_species.emplace_back(name);
        }
      }

      return unknown_species;
    }
  }  // namespace development
}  // namespace mechanism_configuration

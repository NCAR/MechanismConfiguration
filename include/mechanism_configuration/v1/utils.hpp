// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/parse_status.hpp>
#include <mechanism_configuration/v1/model_types.hpp>
#include <mechanism_configuration/v1/reaction_types.hpp>
#include <mechanism_configuration/v1/types.hpp>
#include <mechanism_configuration/v1/validation.hpp>

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mechanism_configuration
{
  namespace v1
  {
    struct DuplicateEntryInfo
    {
      std::string name;
      std::vector<YAML::Node> nodes;
    };

    std::unordered_map<std::string, std::string> GetComments(const YAML::Node& object);

    template<typename T>
    std::vector<DuplicateEntryInfo> FindDuplicateObjectsByName(const std::vector<std::pair<T, YAML::Node>>& collection)
    {
      std::unordered_map<std::string, std::vector<YAML::Node>> name_to_nodes;

      for (const auto& [elem, node] : collection)
      {
        name_to_nodes[elem.name].push_back(node);
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

    template<typename SpeciesType>
    std::vector<std::string> FindUnknownSpecies(const std::vector<std::string>& requested_species, 
        const std::vector<SpeciesType>& existing_species)
    {
      std::unordered_set<std::string> existing_names;

      if constexpr (std::is_same<SpeciesType, std::string>::value)
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
  }  // namespace v1
}  // namespace mechanism_configuration

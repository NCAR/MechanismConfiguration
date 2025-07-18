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

    std::unordered_map<std::string, std::string> GetComments(const YAML::Node& object);

    template<typename T>
    std::vector<std::string> FindDuplicateObjectsByName(const std::vector<T>& collection)
    {

      std::unordered_map<std::string, size_t> name_count;
      std::vector<std::string> duplicates;

      for (const auto& elem : collection)
      {
        name_count[elem.name]++;
      }

      for (const auto& pair : name_count)
      {
        if (pair.second > 1)
        {
          duplicates.push_back(pair.first);
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

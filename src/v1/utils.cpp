// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validation_keys.hpp>
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <functional>
#include <optional>
#include <unordered_set>

namespace mechanism_configuration
{
  namespace v1
  {
    YAML::Node AsSequence(const YAML::Node& node)
    {
      if (node.IsSequence())
        return node;

      YAML::Node sequence;
      sequence.push_back(node);

      return sequence;
    }

    std::string GetReactionComponentName(const YAML::Node& component)
    {
      // A component may be given as a bare string (shorthand for its name),
      // or as an object keyed by the canonical `name` or the legacy `species name`.
      if (component.IsScalar())
        return component.as<std::string>();
      if (component[validation::name])
        return component[validation::name].as<std::string>();
      return component[validation::species_name].as<std::string>();
    }

    void AppendFilePath(const std::string& config_path, Errors& errors)
    {
      for (auto& error : errors)
      {
        error.second = config_path + ":" + error.second;
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

  }  // namespace v1
}  // namespace mechanism_configuration

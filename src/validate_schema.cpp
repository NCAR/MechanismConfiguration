// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/validate_schema.hpp>

#include <iostream>

namespace mechanism_configuration
{
  Errors ValidateSchema(
      const YAML::Node& object,
      const std::vector<std::string>& required_keys,
      const std::vector<std::string>& optional_keys)
  {
    Errors errors;
    std::string line = std::to_string(object.Mark().line + 1);
    std::string column = std::to_string(object.Mark().column + 1);
    if (!object || object.IsNull())
    {
      errors.push_back({ ConfigParseStatus::RequiredKeyNotFound, line + ":" + column + ": error: Object is null" });
      return errors;
    }

    // Collect all keys from the object
    std::vector<std::string> object_keys;
    for (const auto& key : object)
    {
      object_keys.push_back(key.first.as<std::string>());
    }

    // Sort keys for comparison
    auto sorted_required_keys = required_keys;
    auto sorted_optional_keys = optional_keys;
    std::sort(object_keys.begin(), object_keys.end());
    std::sort(sorted_required_keys.begin(), sorted_required_keys.end());
    std::sort(sorted_optional_keys.begin(), sorted_optional_keys.end());

    // Find missing required keys
    std::vector<std::string> missing_keys;
    std::set_difference(
        sorted_required_keys.begin(),
        sorted_required_keys.end(),
        object_keys.begin(),
        object_keys.end(),
        std::back_inserter(missing_keys));

    for (const auto& key : missing_keys)
    {
      errors.push_back(
          { ConfigParseStatus::RequiredKeyNotFound, line + ":" + column + ": error: Missing required key '" + key + "'" });
    }

    // Find keys that are neither required nor optional
    std::vector<std::string> extra_keys;
    std::set_difference(
        object_keys.begin(),
        object_keys.end(),
        sorted_required_keys.begin(),
        sorted_required_keys.end(),
        std::back_inserter(extra_keys));

    std::vector<std::string> invalid_keys;
    std::set_difference(
        extra_keys.begin(),
        extra_keys.end(),
        sorted_optional_keys.begin(),
        sorted_optional_keys.end(),
        std::back_inserter(invalid_keys));

    // now, anything left must be standard comment starting with __
    for (auto& key : invalid_keys)
    {
      if (key.find("__") == std::string::npos)
      {
        std::string line = std::to_string(object[key].Mark().line + 1);
        std::string column = std::to_string(object[key].Mark().column + 1);
        errors.push_back(
            { ConfigParseStatus::InvalidKey, line + ":" + column + ": error: Non-standard key '" + key + "' found" });
      }
    }

    return errors;
  }
}  // namespace mechanism_configuration

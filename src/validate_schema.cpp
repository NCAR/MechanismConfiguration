// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/error_location.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <iostream>
#include <format>

namespace mechanism_configuration
{
  Errors ValidateSchema(
      const YAML::Node& object,
      const std::vector<std::string>& required_keys,
      const std::vector<std::string>& optional_keys)
  {
    Errors errors;
    ErrorLocation error_location{ object.Mark().line, object.Mark().column };

    if (!object || object.IsNull())
    {
      std::string message = std::format("{} error: Object is null.", error_location);
      errors.push_back({ ConfigParseStatus::EmptyObject, message });

      return errors;
    }

    // Collect all keys from the object
    std::vector<std::string> object_keys;
    for (const auto& key : object)
    {
      object_keys.emplace_back(key.first.as<std::string>());
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
      std::string message = std::format("{} error: Required key '{}' is missing.", error_location, key);
      errors.push_back({ ConfigParseStatus::RequiredKeyNotFound, message });
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
        std::string message = std::format("{} error: Non-standard key '{}' found.", error_location, key);
        errors.push_back({ ConfigParseStatus::InvalidKey, message });
      }
    }

    return errors;
  }
}  // namespace mechanism_configuration

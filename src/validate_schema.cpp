// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/format_compat.hpp>
#include "detail/validate_schema.hpp"

#include <iostream>

namespace mechanism_configuration
{
  Errors ValidateSchema(
      const YAML::Node& object,
      const std::vector<std::string_view>& required_keys,
      const std::vector<std::string_view>& optional_keys,
      const std::vector<std::vector<std::string_view>>& exactly_one_of)
  {
    Errors errors;
    ErrorLocation error_location{ object.Mark().line, object.Mark().column };

    if (!object || object.IsNull())
    {
      std::string message = mc_fmt::format("{} error: Object is null.", error_location);
      errors.push_back({ ErrorCode::EmptyObject, message });

      return errors;
    }

    // Collect all keys from the object
    std::vector<std::string> object_keys;
    for (const auto& key : object)
    {
      object_keys.emplace_back(key.first.as<std::string>());
    }

    // Sort keys for comparison (own the strings so comparisons stay homogeneous)
    std::vector<std::string> sorted_required_keys(required_keys.begin(), required_keys.end());
    std::vector<std::string> sorted_optional_keys(optional_keys.begin(), optional_keys.end());
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
      std::string message = mc_fmt::format("{} error: Required key '{}' is missing.", error_location, key);
      errors.push_back({ ErrorCode::RequiredKeyNotFound, message });
    }

    // Exactly-one-of groups: exactly one member of each group must be present.
    // Group members also count as allowed keys, so they are never flagged as invalid.
    for (const auto& group : exactly_one_of)
    {
      std::vector<std::string> present;
      for (const auto& key : group)
      {
        std::string key_str(key);
        sorted_optional_keys.push_back(key_str);
        if (std::find(object_keys.begin(), object_keys.end(), key_str) != object_keys.end())
          present.push_back(key_str);
      }

      if (present.size() != 1)
      {
        std::string joined;
        for (size_t i = 0; i < group.size(); ++i)
        {
          joined += "'" + std::string(group[i]) + "'";
          if (i + 1 < group.size())
            joined += ", ";
        }

        if (present.empty())
        {
          std::string message = mc_fmt::format("{} error: Exactly one of {} is required.", error_location, joined);
          errors.push_back({ ErrorCode::RequiredKeyNotFound, message });
        }
        else
        {
          std::string message = mc_fmt::format("{} error: Only one of {} may be specified.", error_location, joined);
          errors.push_back({ ErrorCode::MutuallyExclusiveOption, message });
        }
      }
    }
    std::sort(sorted_optional_keys.begin(), sorted_optional_keys.end());

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
        std::string message = mc_fmt::format("{} error: Non-standard key '{}' found.", error_location, key);
        errors.push_back({ ErrorCode::InvalidKey, message });
      }
    }

    return errors;
  }
}  // namespace mechanism_configuration

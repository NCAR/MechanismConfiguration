// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/types/property_value.hpp>

#include <detail/error_format.hpp>
#include <detail/location.hpp>
#include <detail/v1/keys.hpp>
#include <yaml-cpp/yaml.h>

#include <exception>
#include <filesystem>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace mechanism_configuration::v1
{
  /// @brief The items that the parser cannot build, and the errors that tell why.
  struct LeftOutItems
  {
    /// @brief The JSON Pointer of each item that the mechanism does not have
    std::set<std::string, std::less<>> paths;
    Errors errors;
  };

  /// @brief Records that the item at `path` is left out. `reason` follows the message.
  inline void LeaveOut(
      const YAML::Node& item,
      const std::string& path,
      std::string_view what,
      const std::string& reason,
      LeftOutItems& left_out)
  {
    left_out.paths.insert(path);
    left_out.errors.push_back(
        { ErrorCode::ItemLeftOut,
          ToString(LocationOf(item)) + " error: The " + std::string(what) + " cannot be built, so it is left out" + reason +
              ".",
          path });
  }

  /// @brief Builds one item with `build`. If the build throws, the item is left out, and
  ///        `left_out` gets its path and an error.
  /// @param item The YAML node of the item, for the location in the error
  /// @param path The JSON Pointer to the item
  /// @param what The kind of the item, for example "species"
  template<typename Build>
  void
  BuildItem(const YAML::Node& item, const std::string& path, std::string_view what, LeftOutItems& left_out, Build&& build)
  {
    try
    {
      build();
    }
    catch (const YAML::Exception&)
    {
      // A schema error of the item tells the cause, so the yaml-cpp message is not added.
      LeaveOut(item, path, what, "", left_out);
    }
    catch (const std::exception& e)
    {
      LeaveOut(item, path, what, std::string(": ") + e.what(), left_out);
    }
  }

  /// @brief Ensures a YAML node is treated as a sequence
  /// @param node The YAML node to convert
  /// @return A YAML sequence node containing the original node(s)
  YAML::Node AsSequence(const YAML::Node& node);

  /// @brief The JSON Pointer to item `index` of AsSequence(node). A node that is not a sequence
  ///        is its own only item, so its path is `path`.
  /// @param node The YAML node that AsSequence was called on
  /// @param path The JSON Pointer to `node`
  /// @param index The index of the item in AsSequence(node)
  std::string ItemPath(const YAML::Node& node, const std::string& path, std::size_t index);

  /// @brief Puts the configuration file path in front of each error message.
  void AppendFilePath(const std::string& config_path, Errors& errors);

  /// @brief Collects the unknown properties of an object: every key that starts with two underscores (__).
  ///        Values keep their type and structure.
  types::UnknownProperties GetComments(const YAML::Node& object);

  /// @brief Reads a named entry's name, accepting either a bare-string shorthand or an object
  ///        keyed by the canonical `name` or the legacy `species name` alias (v1 files). Used for
  ///        species, phase-species, and reaction component entries.
  /// @note Assumes the entry has already been validated to contain exactly one of them.
  std::string GetComponentName(const YAML::Node& component);

}  // namespace mechanism_configuration::v1

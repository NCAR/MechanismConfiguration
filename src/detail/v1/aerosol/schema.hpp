// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

#include <string>

namespace mechanism_configuration::v1
{
  /// @brief Schema-validates each entry of the aerosol representations section.
  /// @param representations_list YAML node containing the representation entries
  /// @param path JSON Pointer to the section. The errors of an item point to that item.
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckAerosolRepresentationsSchema(const YAML::Node& representations_list, const std::string& path);

  /// @brief Schema-validates each entry of the aerosol processes section which mixes
  ///        process and constraint types. Structural only; cross-references against species and
  ///        phases (membership, sourced properties) are checked separately by ValidateAerosolSemantics.
  /// @param processes_list YAML node containing the process/constraint entries
  /// @param path JSON Pointer to the section. The errors of an item point to that item.
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckAerosolProcessesSchema(const YAML::Node& processes_list, const std::string& path);

}  // namespace mechanism_configuration::v1

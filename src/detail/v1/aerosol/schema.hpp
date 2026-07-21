// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration::v1
{
  /// @brief Schema-validates each entry of the aerosol representations section.
  /// @param representations_list YAML node containing the representation entries
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckAerosolRepresentationsSchema(const YAML::Node& representations_list);

  /// @brief Schema-validates each entry of the aerosol processes section which mixes
  ///        process and constraint types. Structural only; cross-references against species and
  ///        phases (membership, sourced properties) are checked separately by ValidateAerosolSemantics.
  /// @param processes_list YAML node containing the process/constraint entries
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckAerosolProcessesSchema(const YAML::Node& processes_list);

}  // namespace mechanism_configuration::v1

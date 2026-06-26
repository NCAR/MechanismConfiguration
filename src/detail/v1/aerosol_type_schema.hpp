// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/types.hpp>

#include <yaml-cpp/yaml.h>

#include <vector>

namespace mechanism_configuration::v1
{
  /// @brief Schema-validates each entry of the aerosol representations section.
  /// @param representations_list YAML node containing the representation entries
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckAerosolRepresentationsSchema(const YAML::Node& representations_list);

  /// @brief Schema-validates each entry of the aerosol processes section which mixes
  ///        process and constraint types.
  /// @param processes_list YAML node containing the process/constraint entries
  /// @param species Parsed top-level species, used to validate cross-references such as a Henry's-law
  ///        equilibrium's solvent species carrying a molecular weight
  /// @param phases Parsed phases, used to validate cross-references such as a phase-transfer's
  ///        gas-phase species carrying a diffusion coefficient, or a solvent's density
  /// @return List of structural errors, or empty if all entries conform
  Errors CheckAerosolProcessesSchema(
      const YAML::Node& processes_list,
      const std::vector<types::Species>& species,
      const std::vector<types::Phase>& phases);

}  // namespace mechanism_configuration::v1

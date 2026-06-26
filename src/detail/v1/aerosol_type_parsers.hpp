// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/aerosol_types.hpp>
#include <mechanism_configuration/types.hpp>

#include <yaml-cpp/yaml.h>

#include <map>
#include <string>
#include <vector>

// Each Parse function assumes its input has already been validated by the matching CheckAerosol*Schema.
namespace mechanism_configuration::v1
{
  // ----------------------------------------
  // Rate constant parsers
  // ----------------------------------------

  types::HenryLawConstant ParseHenryLawConstant(const YAML::Node& object);
  types::ArrheniusReferenceTemperature ParseArrheniusReferenceTemperature(const YAML::Node& object);

  /// @brief Parses a bare Arrhenius rate-constant block (A, B, C, D, E).
  types::Arrhenius ParseArrhenius(const YAML::Node& object);

  /// @brief Parses one rate-constant block into the RateConstant variant, dispatching on its
  ///        inner `type` (ARRHENIUS -> Arrhenius, ARRHENIUS_REFERENCE_TEMPERATURE -> Arrhenius reference temperature).
  types::RateConstant ParseRateConstant(const YAML::Node& object);

  /// @brief Parses a per-representation rate-constant map, e.g. { "CLOUD": { ... } }.
  /// @return Map keyed by representation name to its rate constant.
  std::map<std::string, types::RateConstant> ParseRateConstantMap(const YAML::Node& object);

  // ----------------------------------------
  // Representation parsers
  // ----------------------------------------

  types::UniformSection ParseUniformSection(const YAML::Node& object);
  types::SingleMomentMode ParseSingleMomentMode(const YAML::Node& object);
  types::TwoMomentMode ParseTwoMomentMode(const YAML::Node& object);

  // ----------------------------------------
  // Process parsers
  // ----------------------------------------

  types::HenryLawPhaseTransfer ParseHenryLawPhaseTransfer(const YAML::Node& object);
  types::DissolvedReaction ParseDissolvedReaction(const YAML::Node& object);
  types::DissolvedReversibleReaction ParseDissolvedReversibleReaction(const YAML::Node& object);

  // ----------------------------------------
  // Constraint parsers
  // ----------------------------------------

  types::HenryLawEquilibrium ParseHenryLawEquilibrium(const YAML::Node& object);
  types::DissolvedEquilibrium ParseDissolvedEquilibrium(const YAML::Node& object);
  types::LinearConstraint ParseLinearConstraint(const YAML::Node& object);

  // ----------------------------------------
  // Container parsers
  // ----------------------------------------

  std::vector<types::Representation> ParseAerosolRepresentations(const YAML::Node& objects);

  /// @brief Parses the aerosol processes sequence into the processes or constraints
  void ParseAerosolProcesses(const YAML::Node& objects, types::Aerosol& aerosol);

}  // namespace mechanism_configuration::v1

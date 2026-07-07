// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/types/aerosol.hpp>
#include <mechanism_configuration/types/reactions.hpp>
#include <mechanism_configuration/types/species.hpp>

#include <yaml-cpp/yaml.h>

#include <map>
#include <string>
#include <vector>

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

  /// @brief Parses a Henry's-law phase transfer. The diffusion coefficient is sourced from the
  ///        gas-phase species' definition in `phases`.
  types::HenryLawPhaseTransfer ParseHenryLawPhaseTransfer(const YAML::Node& object, const std::vector<types::Phase>& phases);
  types::DissolvedReaction ParseDissolvedReaction(const YAML::Node& object);
  types::DissolvedReversibleReaction ParseDissolvedReversibleReaction(const YAML::Node& object);

  // ----------------------------------------
  // Constraint parsers
  // ----------------------------------------

  /// @brief Parses a Henry's-law equilibrium. The solvent's molecular weight is sourced from the
  ///        species section and its density from the condensed phase.
  types::HenryLawEquilibrium ParseHenryLawEquilibrium(
      const YAML::Node& object,
      const std::vector<types::Species>& species,
      const std::vector<types::Phase>& phases);
  types::DissolvedEquilibrium ParseDissolvedEquilibrium(const YAML::Node& object);
  types::LinearConstraint ParseLinearConstraint(const YAML::Node& object);

  // ----------------------------------------
  // Container parsers
  // ----------------------------------------

  std::vector<types::Representation> ParseAerosolRepresentations(const YAML::Node& objects);

  /// @brief Parses the full aerosol section (representations plus the mixed processes/constraints
  ///        list) into a single Aerosol container.
  /// @param species Parsed top-level species, used to source per-species values such as a Henry's-law
  ///        equilibrium solvent's molecular weight
  /// @param phases Parsed phases, used to source per-species values such as a phase-transfer's
  ///        gas-phase diffusion coefficient or a solvent's density
  types::Aerosol ParseAerosol(
      const YAML::Node& object,
      const std::vector<types::Species>& species,
      const std::vector<types::Phase>& phases);

}  // namespace mechanism_configuration::v1

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/types/species.hpp>

#include <optional>
#include <string>
#include <vector>

namespace mechanism_configuration::v1
{
  /// @brief Looks up the diffusion coefficient defined for a species within a phase.
  /// @param phases Parsed phases to search
  /// @param phase_name Name of the phase that should contain the species
  /// @param species_name Name of the species whose diffusion coefficient is requested
  /// @return The diffusion coefficient, or nullopt if the phase/species is not found or the
  ///         species has no diffusion coefficient defined.
  std::optional<double> FindPhaseSpeciesDiffusionCoefficient(
      const std::vector<types::Phase>& phases,
      const std::string& phase_name,
      const std::string& species_name);

  /// @brief Looks up the density defined for a species within a phase.
  /// @return The density, or nullopt if the phase/species is not found or the species has no
  ///         density defined.
  std::optional<double> FindPhaseSpeciesDensity(
      const std::vector<types::Phase>& phases,
      const std::string& phase_name,
      const std::string& species_name);

  /// @brief Looks up the molecular weight defined for a top-level species.
  /// @return The molecular weight, or nullopt if the species is not found or has none defined.
  std::optional<double> FindSpeciesMolecularWeight(
      const std::vector<types::Species>& species,
      const std::string& species_name);

}  // namespace mechanism_configuration::v1

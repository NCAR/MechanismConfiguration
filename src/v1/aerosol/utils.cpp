// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/aerosol/utils.hpp"

#include <optional>

namespace mechanism_configuration::v1
{
  std::optional<double> FindPhaseSpeciesDiffusionCoefficient(
      const std::vector<types::Phase>& phases,
      const std::string& phase_name,
      const std::string& species_name)
  {
    for (const auto& phase : phases)
    {
      if (phase.name != phase_name)
        continue;
      for (const auto& species : phase.species)
      {
        if (species.name == species_name)
          return species.diffusion_coefficient;
      }
    }
    return std::nullopt;
  }

  std::optional<double> FindPhaseSpeciesDensity(
      const std::vector<types::Phase>& phases,
      const std::string& phase_name,
      const std::string& species_name)
  {
    for (const auto& phase : phases)
    {
      if (phase.name != phase_name)
        continue;
      for (const auto& species : phase.species)
      {
        if (species.name == species_name)
          return species.density;
      }
    }
    return std::nullopt;
  }

  std::optional<double> FindSpeciesMolecularWeight(
      const std::vector<types::Species>& species,
      const std::string& species_name)
  {
    for (const auto& s : species)
    {
      if (s.name == species_name)
        return s.molecular_weight;
    }
    return std::nullopt;
  }

}  // namespace mechanism_configuration::v1

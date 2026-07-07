// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/mechanism_version.hpp>
#include <mechanism_configuration/types/aerosol.hpp>
#include <mechanism_configuration/types/emissions.hpp>
#include <mechanism_configuration/types/reactions.hpp>
#include <mechanism_configuration/types/species.hpp>

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <variant>

namespace mechanism_configuration
{
  /// @brief Represents a full mechanism definition
  struct Mechanism
  {
    /// @brief Mechanism name (optional)
    std::string name;
    /// @brief Species list
    std::vector<types::Species> species;
    /// @brief Phases list
    std::vector<types::Phase> phases;
    /// @brief Represents a collection of different reaction types, each stored in a vector
    ///        corresponding to a specific mechanism
    types::Reactions reactions;
    /// @brief Aerosol representations, processes, and constraints
    types::Aerosol aerosol;
    /// @brief Version of the mechanism configuration format used, in major.minor.patch format
    Version version;
    /// @brief Relative tolerance for solver (optional, default: 1e-6)
    double relative_tolerance{ 1e-6 };
    /// @brief Emissions configuration (optional)
    std::optional<types::EmissionsConfig> emissions;
  };
}  // namespace mechanism_configuration
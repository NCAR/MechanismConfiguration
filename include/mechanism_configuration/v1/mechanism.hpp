// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/v1/aerosol_types.hpp>
#include <mechanism_configuration/v1/reaction_types.hpp>
#include <mechanism_configuration/v1/types.hpp>

#include <string>
#include <vector>

namespace mechanism_configuration
{
  namespace v1
  {
    namespace types
    {

      struct Mechanism : public ::mechanism_configuration::Mechanism
      {
        /// @brief Mechanism name
        /// @note  This is optional.
        std::string name;
        /// @brief Represents species
        std::vector<Species> species;
        /// @brief Represents phases
        std::vector<Phase> phases;
        /// @brief Represents a collection of different reaction types, each stored in a vector
        ///        corresponding to a specific mechanism
        Reactions reactions;
        /// @brief Aerosol representations (optional, for MIAM-enabled configs)
        std::vector<AerosolRepresentation> aerosol_representations;
        /// @brief Aerosol processes and constraints (optional, for MIAM-enabled configs)
        std::vector<AerosolProcess> aerosol_processes;
      };
    }  // namespace types
  }  // namespace v1
}  // namespace mechanism_configuration
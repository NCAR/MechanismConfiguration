// Copyright (C) 2026 University Corporation for Atmospheric Research
//                     University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/v1/aerosol_types.hpp>
#include <mechanism_configuration/v1/types.hpp>

#include <yaml-cpp/yaml.h>

#include <utility>
#include <vector>

namespace mechanism_configuration
{
  namespace v1
  {
    std::pair<Errors, std::vector<types::AerosolRepresentation>> ParseAerosolRepresentations(
        const YAML::Node& objects,
        const std::vector<types::Phase>& existing_phases);

    std::pair<Errors, std::vector<types::AerosolProcess>> ParseAerosolProcesses(
        const YAML::Node& objects,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases);

  }  // namespace v1
}  // namespace mechanism_configuration

// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration
{
  namespace development
  {

    Errors ValidateSpecies(const YAML::Node& species_list);

  }  // namespace development
}  // namespace mechanism_configuration
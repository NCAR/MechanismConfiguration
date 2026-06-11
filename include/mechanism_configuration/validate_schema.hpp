// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

#include <string>
#include <vector>

namespace mechanism_configuration
{
  Errors ValidateSchema(
      const YAML::Node& object,
      const std::vector<std::string_view>& required_keys,
      const std::vector<std::string_view>& optional_keys);
}  // namespace mechanism_configuration

// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/parse_status.hpp>

#include <yaml-cpp/yaml.h>

#include <string>
#include <vector>

namespace mechanism_configuration
{
  enum ObjectType {
    String,
  };

  Errors ValidateSchema(const YAML::Node& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys);
  Errors ValidateObjectType(const YAML::Node& object, const std::string& key, ObjectType type);
}  // namespace mechanism_configuration

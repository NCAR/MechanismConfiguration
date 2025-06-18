// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/parse_status.hpp>

#include <yaml-cpp/yaml.h>

#include <string>
#include <utility>
#include <vector>

namespace mechanism_configuration
{
  using Errors = std::vector<std::pair<ConfigParseStatus, std::string>>;
}  // namespace mechanism_configuration
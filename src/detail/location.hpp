// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

namespace mechanism_configuration
{
  inline ErrorLocation LocationOf(const YAML::Node& node)
  {
    return ErrorLocation{ node.Mark().line, node.Mark().column };
  }
}  // namespace mechanism_configuration

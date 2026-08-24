// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "detail/location.hpp"
#include "detail/semantics/core.hpp"

#include <yaml-cpp/yaml.h>

#include <vector>

namespace mechanism_configuration
{
  inline void CollectMapComponents(const YAML::Node& node, std::vector<semantics::NamedRef>& out)
  {
    if (!node)
      return;
    for (auto it = node.begin(); it != node.end(); ++it)
      out.push_back({ it->first.as<std::string>(), LocationOf(it->first) });
  }

  inline void CollectBareComponent(const YAML::Node& node, std::vector<semantics::NamedRef>& out)
  {
    if (!node)
      return;
    out.push_back({ node.as<std::string>(), LocationOf(node) });
  }
}  // namespace mechanism_configuration

// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/compatability.hpp>

namespace mechanism_configuration
{
  namespace development
  {
    void BackwardCompatibleSpeciesName(YAML::Node& node)
    {
      if (node.IsMap())
      {
        // If this node has "species name" but not "name", rename it
        if (node["species name"] && !node["name"])
        {
          node["name"] = node["species name"];
          node.remove("species name");
        }
      }
      else if (node.IsSequence())
      {
        for (auto item : node)
        {
          BackwardCompatibleSpeciesName(item);
        }
      }
    }
  }  // namespace development
}  // namespace mechanism_configuration
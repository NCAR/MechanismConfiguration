// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/aerosol_types.hpp>
#include <mechanism_configuration/types.hpp>

#include <filesystem>
#include <iostream>
#include <string>
#include <variant>

namespace mechanism_configuration
{
// glibc defines `major` and `minor` as macros, which conflict with the `Version` struct fields.
// Temporarily undefine the macros before the struct definition and restore them afterward.
// https://stackoverflow.com/a/22253389/5217293
#pragma push_macro("major")
#undef major
#pragma push_macro("minor")
#undef minor

  struct Version
  {
    unsigned int major;
    unsigned int minor;
    unsigned int patch;

    Version()
        : major(0),
          minor(0),
          patch(0)
    {
    }

    Version(unsigned int major, unsigned int minor, unsigned int patch)
        : major(major),
          minor(minor),
          patch(patch)
    {
    }

    Version(std::string version)
    {
      std::string delimiter = ".";
      size_t pos = 0;
      int i = 0;
      while ((pos = version.find(delimiter)) != std::string::npos)
      {
        std::string token = version.substr(0, pos);
        switch (i)
        {
          case 0: major = std::stoi(token); break;
          case 1: minor = std::stoi(token); break;
        }
        version.erase(0, pos + delimiter.length());
        i++;
      }
      // The remaining part is the patch version
      if (!version.empty())
      {
        patch = std::stoi(version);
      }
    }

    std::string to_string() const
    {
      return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }
  };

#pragma pop_macro("minor")
#pragma pop_macro("major")

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
  };
}  // namespace mechanism_configuration
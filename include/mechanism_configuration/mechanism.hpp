// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/parse_status.hpp>

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <variant>

namespace mechanism_configuration
{
// Trying to build on linux for the python release, I learned that glibc had a bug which defined
// a macro called major and minor. This caused a conflict with the Version struct. To fix this, I
// undefine the macros before defining the struct and then redefine them after the struct.
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

  struct Mechanism
  {
    Version version;

    Mechanism(Version version)
        : version(version)
    {
    }
    Mechanism()
        : version()
    {
    }
    virtual ~Mechanism() = default;
  };

  using GlobalMechanism = ::mechanism_configuration::Mechanism;
}  // namespace mechanism_configuration
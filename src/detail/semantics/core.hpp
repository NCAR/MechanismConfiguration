// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <optional>
#include <string>

namespace mechanism_configuration::semantics
{
  // A referenced name and its source location. Parsed documents populate the location
  // (line:col for error reporting) and in-code Mechanism leaves it empty.
  struct NamedRef
  {
    std::string name;
    std::optional<ErrorLocation> location;
  };

}  // namespace mechanism_configuration::semantics

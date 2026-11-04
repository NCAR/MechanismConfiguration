// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <string>

namespace mechanism_configuration
{
  inline std::string ToString(const ErrorLocation& location)
  {
    return std::to_string(location.line) + ":" + std::to_string(location.column);
  }
}  // namespace mechanism_configuration

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <ostream>

namespace mechanism_configuration
{
  // For Google Test printing
  inline void PrintTo(const ErrorCode& status, std::ostream* os)
  {
    *os << ErrorCodeToString(status);
  }
}  // namespace mechanism_configuration

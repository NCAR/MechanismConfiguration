// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>

#include <ostream>

namespace mechanism_configuration
{
  // Kept internal (not in the public errors.hpp). Only the .cpp files that build
  // error-message strings need it.
  inline std::ostream& operator<<(std::ostream& out, const ErrorLocation& loc)
  {
    return out << loc.line << ':' << loc.column;
  }
}  // namespace mechanism_configuration

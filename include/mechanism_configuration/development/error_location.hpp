// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <format>
#include <string>

namespace mechanism_configuration
{
  namespace development
  {
    struct ErrorLocation
    {
      int line, column;

      /// @brief Constructor that adjusts 0-based indices to 1-based
      /// @param line_number 0-based line number (from YAML::Mark)
      /// @param column_number 0-based column number (from YAML::Mark)
      ErrorLocation(int line_number, int column_number)
          : line(line_number + 1),
            column(column_number + 1)
      {
      }
    };
  }  // namespace development
}  // namespace mechanism_configuration

namespace std
{
  template<>
  struct formatter<mechanism_configuration::development::ErrorLocation> : std::formatter<std::string>
  {
    auto format(const mechanism_configuration::development::ErrorLocation& loc, format_context& ctx) const
    {
      return formatter<std::string>::format(std::format("{}:{}", loc.line, loc.column), ctx);
    }
  };
}  // namespace std
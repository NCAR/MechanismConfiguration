// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/format_compat.hpp>
#include <string>

namespace mechanism_configuration
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
}  // namespace mechanism_configuration

#ifdef MECH_CONFIG_USE_FMT
template <>
struct fmt::formatter<mechanism_configuration::ErrorLocation>
{
  constexpr auto parse(fmt::format_parse_context& ctx) const
  {
    return ctx.begin();
  }
  template <class FormatContext>
  auto format(const mechanism_configuration::ErrorLocation& loc, FormatContext& ctx) const
  {
    return fmt::format_to(ctx.out(), "{}:{}", loc.line, loc.column);
  }
};
#else
template <>
struct std::formatter<mechanism_configuration::ErrorLocation>
{
  constexpr auto parse(std::format_parse_context& ctx) const
  {
    return ctx.begin();
  }
  template <class FormatContext>
  auto format(const mechanism_configuration::ErrorLocation& loc, FormatContext& ctx) const
  {
    return std::format_to(ctx.out(), "{}:{}", loc.line, loc.column);
  }
};
#endif
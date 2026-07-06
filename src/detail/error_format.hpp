// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>

// Formatter for ErrorLocation. Kept internal (not in the public errors.hpp) so that
// fmt / <format> is not pulled into the library's public interface. Only the .cpp
// files that build error-message strings need this, so it lives under src/detail.
#ifdef MECH_CONFIG_USE_FMT
template<>
struct fmt::formatter<mechanism_configuration::ErrorLocation>
{
  constexpr auto parse(fmt::format_parse_context& ctx) const
  {
    return ctx.begin();
  }
  template<class FormatContext>
  auto format(const mechanism_configuration::ErrorLocation& loc, FormatContext& ctx) const
  {
    return fmt::format_to(ctx.out(), "{}:{}", loc.line, loc.column);
  }
};
#else
template<>
struct std::formatter<mechanism_configuration::ErrorLocation>
{
  constexpr auto parse(std::format_parse_context& ctx) const
  {
    return ctx.begin();
  }
  template<class FormatContext>
  auto format(const mechanism_configuration::ErrorLocation& loc, FormatContext& ctx) const
  {
    return std::format_to(ctx.out(), "{}:{}", loc.line, loc.column);
  }
};
#endif

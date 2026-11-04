// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

// A small replacement for std::format, used only to build error messages.
// It supports the "{}" and "{:.Nf}" placeholders. Each argument must support operator<<.
// The library does not use std::format or {fmt}, because many HPC compilers use a
// libstdc++ that does not have <format>.

#include <cstddef>
#include <ios>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace mc_fmt
{
  namespace detail
  {
    inline void WriteArgument(std::ostringstream&, std::string_view, std::size_t)
    {
      throw std::invalid_argument("mc_fmt::format: too few arguments for the format string");
    }

    template<class T, class... Rest>
    void WriteArgument(std::ostringstream& out, std::string_view spec, std::size_t index, const T& arg, const Rest&... rest)
    {
      if (index > 0)
      {
        WriteArgument(out, spec, index - 1, rest...);
        return;
      }
      if (spec.empty())
      {
        out << arg;
        return;
      }
      if (spec.size() >= 3 && spec[0] == '.' && spec.back() == 'f')
      {
        const auto old_flags = out.flags();
        const auto old_precision = out.precision();
        out << std::fixed;
        out.precision(std::stoi(std::string(spec.substr(1, spec.size() - 2))));
        out << arg;
        out.flags(old_flags);
        out.precision(old_precision);
        return;
      }
      throw std::invalid_argument("mc_fmt::format: unsupported format spec '" + std::string(spec) + "'");
    }
  }  // namespace detail

  template<class... Args>
  std::string format(std::string_view format_string, const Args&... args)
  {
    std::ostringstream out;
    std::size_t next_argument = 0;
    for (std::size_t i = 0; i < format_string.size(); ++i)
    {
      const char c = format_string[i];
      if (c == '{' && i + 1 < format_string.size() && format_string[i + 1] == '{')
      {
        out << '{';
        ++i;
      }
      else if (c == '}' && i + 1 < format_string.size() && format_string[i + 1] == '}')
      {
        out << '}';
        ++i;
      }
      else if (c == '{')
      {
        const std::size_t close = format_string.find('}', i);
        if (close == std::string_view::npos)
          throw std::invalid_argument("mc_fmt::format: unmatched '{'");
        std::string_view spec = format_string.substr(i + 1, close - i - 1);
        if (!spec.empty() && spec[0] == ':')
          spec.remove_prefix(1);
        detail::WriteArgument(out, spec, next_argument++, args...);
        i = close;
      }
      else
      {
        out << c;
      }
    }
    return out.str();
  }
}  // namespace mc_fmt

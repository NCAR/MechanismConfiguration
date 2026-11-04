// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <detail/error_format.hpp>

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

TEST(FormatCompat, ReplacesPlaceholdersInOrder)
{
  EXPECT_EQ(mc_fmt::format("{} and {}", "a", std::string("b")), "a and b");
  EXPECT_EQ(mc_fmt::format("count {}", 3), "count 3");
  EXPECT_EQ(mc_fmt::format("no placeholders"), "no placeholders");
}

TEST(FormatCompat, FixedPrecision)
{
  EXPECT_EQ(mc_fmt::format("sum to {:.4f}, then {}", 1.1, 2), "sum to 1.1000, then 2");
}

TEST(FormatCompat, EscapedBraces)
{
  EXPECT_EQ(mc_fmt::format("{{literal}} {}", 1), "{literal} 1");
}

TEST(FormatCompat, ErrorLocation)
{
  mechanism_configuration::ErrorLocation location(4, 9);
  EXPECT_EQ(mc_fmt::format("{} error: x", location), "5:10 error: x");
}

TEST(FormatCompat, TooFewArgumentsThrows)
{
  EXPECT_THROW(mc_fmt::format("{} {}", 1), std::invalid_argument);
}

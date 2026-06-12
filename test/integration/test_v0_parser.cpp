// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v0/parser.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

using namespace mechanism_configuration;

TEST(V0Parser, ParsesFullV0Configuration)
{
  v0::Parser parser;
  for (const auto& extension : { std::string(".yaml"), std::string(".json") })
  {
    auto parsed = parser.Parse("examples/v0/config" + extension);
    ASSERT_TRUE(parsed);

    Mechanism mechanism = *parsed;
    EXPECT_EQ(mechanism.reactions.user_defined.size(), 4);
    EXPECT_EQ(mechanism.reactions.arrhenius.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe.size(), 1);
    EXPECT_EQ(mechanism.reactions.ternary_chemical_activation.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched.size(), 2);
    EXPECT_EQ(mechanism.reactions.tunneling.size(), 2);
    EXPECT_EQ(mechanism.reactions.surface.size(), 1);

    EXPECT_EQ(mechanism.version.major, 0);
  }
}

TEST(V0Parser, ConfigPathIsDirectory)
{
  v0::Parser parser;
  auto parsed = parser.Parse(std::filesystem::path("examples/v0"));
  ASSERT_TRUE(parsed);

  Mechanism mechanism = *parsed;
  EXPECT_EQ(mechanism.version.major, 0);
  EXPECT_EQ(mechanism.reactions.user_defined.size(), 4);
  EXPECT_EQ(mechanism.reactions.arrhenius.size(), 1);
  EXPECT_EQ(mechanism.reactions.troe.size(), 1);
  EXPECT_EQ(mechanism.reactions.ternary_chemical_activation.size(), 1);
  EXPECT_EQ(mechanism.reactions.branched.size(), 2);
  EXPECT_EQ(mechanism.reactions.tunneling.size(), 2);
  EXPECT_EQ(mechanism.reactions.surface.size(), 1);
}

TEST(V0Parser, ReportsMissingFile)
{
  v0::Parser parser;
  for (const auto& extension : { std::string(".yaml"), std::string(".json") })
  {
    auto parsed = parser.Parse("examples/_missing_configuration" + extension);
    EXPECT_FALSE(parsed);
    ASSERT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::FileNotFound);
  }
}

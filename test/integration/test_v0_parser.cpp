// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v0/parser.hpp"
#include "utils/print.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

using namespace mechanism_configuration;

TEST(V0Parser, ParsesFullV0Configuration)
{
  v0::Parser parser;
  for (const auto& extension : { std::string(".yaml"), std::string(".json") })
  {
    auto parsed = parser.Parse("examples/v0/config" + extension);
    ASSERT_TRUE(parsed);

    Mechanism mechanism = *parsed.mechanism;
    EXPECT_EQ(mechanism.reactions.user_defined.size(), 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss.size(), 1);
    EXPECT_EQ(mechanism.reactions.emission.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis.size(), 1);
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

  Mechanism mechanism = *parsed.mechanism;
  EXPECT_EQ(mechanism.version.major, 0);
  EXPECT_EQ(mechanism.reactions.user_defined.size(), 1);
  EXPECT_EQ(mechanism.reactions.first_order_loss.size(), 1);
  EXPECT_EQ(mechanism.reactions.emission.size(), 1);
  EXPECT_EQ(mechanism.reactions.photolysis.size(), 1);
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
    ASSERT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].code, ErrorCode::FileNotFound);
  }
}

// The parser leaves out an item with an unknown type, and builds the items after it.
TEST(V0Parser, LeavesOutAnItemWithAnUnknownType)
{
  const auto dir = std::filesystem::temp_directory_path() / "mechanism_configuration_v0_unknown_type";
  std::filesystem::create_directories(dir);
  const auto write = [&](const std::string& name, const std::string& content) { std::ofstream(dir / name) << content; };
  write("config.json", R"({ "camp-files": [ "species.json", "reactions.json" ] })");
  write(
      "species.json", R"({ "camp-data": [ { "name": "A", "type": "CHEM_SPEC" }, { "name": "B", "type": "CHEM_SPEC" } ] })");
  write("reactions.json", R"({ "camp-data": [ { "name": "m", "type": "MECHANISM", "reactions": [
      { "type": "ARRHENIUS", "reactants": { "A": {} }, "products": { "B": {} } },
      { "type": "NOT_A_TYPE" },
      { "type": "ARRHENIUS", "reactants": { "B": {} }, "products": { "A": {} } } ] } ] })");

  auto parsed = v0::Parser{}.Parse(dir);
  std::filesystem::remove_all(dir);

  EXPECT_FALSE(parsed);
  ASSERT_TRUE(parsed.mechanism);
  ASSERT_EQ(parsed.errors.size(), 1);
  EXPECT_EQ(parsed.errors[0].code, ErrorCode::UnknownType);
  EXPECT_EQ(parsed.mechanism->species.size(), 2);
  EXPECT_EQ(parsed.mechanism->reactions.arrhenius.size(), 2);
}

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/parse.hpp>
#include "detail/v1/parser.hpp"

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>

using namespace mechanism_configuration;

namespace
{
  // Parses an in-memory v1 document through the engine (validate, then build).
  std::expected<Mechanism, Errors> ParseString(const std::string& content)
  {
    YAML::Node node = YAML::Load(content);
    v1::Parser parser;
    auto errors = parser.Validate(node, /*read_from_config_file=*/false);
    if (!errors.empty())
      return std::unexpected(std::move(errors));
    return parser.Parse(node);
  }

  const std::string kYamlConfig = R"(
version: 1.0.0
name: Simple Configuration
species:
  - name: A
  - name: B
phases:
  - name: gas
    species:
      - A
      - B
reactions:
  - type: ARRHENIUS
    name: my arrhenius
    gas phase: gas
    reactants:
      - species name: A
        coefficient: 1
    products:
      - species name: B
        coefficient: 1
)";
}  // namespace

TEST(V1Parser, ParsesFullV1Configuration)
{
  auto parsed = parse("examples/v1/full_configuration.json");
  if (!parsed)
    for (const auto& [code, message] : parsed.error())
      std::cout << message << std::endl;
  ASSERT_TRUE(parsed);

  const Mechanism& mechanism = *parsed;
  EXPECT_EQ(mechanism.name, "Full Configuration");
  EXPECT_EQ(mechanism.species.size(), 7);
  EXPECT_EQ(mechanism.phases.size(), 1);

  EXPECT_EQ(mechanism.reactions.arrhenius.size(), 2);
  EXPECT_EQ(mechanism.reactions.branched.size(), 1);
  EXPECT_EQ(mechanism.reactions.emission.size(), 1);
  EXPECT_EQ(mechanism.reactions.first_order_loss.size(), 1);
  EXPECT_EQ(mechanism.reactions.photolysis.size(), 1);
  EXPECT_EQ(mechanism.reactions.surface.size(), 1);
  EXPECT_EQ(mechanism.reactions.taylor_series.size(), 1);
  EXPECT_EQ(mechanism.reactions.troe.size(), 1);
  EXPECT_EQ(mechanism.reactions.ternary_chemical_activation.size(), 1);
  EXPECT_EQ(mechanism.reactions.tunneling.size(), 1);
  EXPECT_EQ(mechanism.reactions.user_defined.size(), 1);

  EXPECT_EQ(mechanism.version.major, 1);
}

TEST(V1Parser, ReportsMissingFile)
{
  for (const auto& extension : { std::string(".yaml"), std::string(".json") })
  {
    auto parsed = parse("examples/_missing_configuration" + extension);
    EXPECT_FALSE(parsed);
    ASSERT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::FileNotFound);
  }
}

TEST(V1Parser, CanParseFromYamlString)
{
  auto parsed = ParseString(kYamlConfig);
  ASSERT_TRUE(parsed);
  EXPECT_EQ(parsed->name, "Simple Configuration");
  EXPECT_EQ(parsed->species.size(), 2);
  EXPECT_EQ(parsed->phases.size(), 1);
  EXPECT_EQ(parsed->reactions.arrhenius.size(), 1);
  EXPECT_EQ(parsed->version.major, 1);
}

TEST(V1Parser, CanParseFromJsonString)
{
  const std::string content = R"({
  "version": "1.0.0",
  "name": "Simple Configuration",
  "species": [ { "name": "A" }, { "name": "B" } ],
  "phases": [ { "name": "gas", "species": [ {"name": "A"}, {"name": "B"} ] } ],
  "reactions": [
    {
      "type": "ARRHENIUS",
      "name": "my arrhenius",
      "gas phase": "gas",
      "reactants": [ { "species name": "A", "coefficient": 1 } ],
      "products": [ { "species name": "B", "coefficient": 1 } ]
    }
  ]
})";
  auto parsed = ParseString(content);
  ASSERT_TRUE(parsed);
  EXPECT_EQ(parsed->name, "Simple Configuration");
  EXPECT_EQ(parsed->species.size(), 2);
  EXPECT_EQ(parsed->reactions.arrhenius.size(), 1);
  EXPECT_EQ(parsed->version.major, 1);
}

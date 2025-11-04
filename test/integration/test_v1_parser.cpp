#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/types.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, ParsesFullV1Configuration)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json" };
  for (auto& extension : extensions)
  {
    std::string path = "examples/v1/full_configuration" + extension;
    auto parsed = parser.Parse(path);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;
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

    EXPECT_EQ(mechanism.species[1].constant_concentration.has_value(), true);
    EXPECT_EQ(mechanism.species[1].constant_concentration.value(), 1.0e19);
    EXPECT_EQ(mechanism.species[2].constant_mixing_ratio.has_value(), true);
    EXPECT_EQ(mechanism.species[2].constant_mixing_ratio.value(), 1.0e-20);
    EXPECT_EQ(mechanism.species[3].is_third_body.has_value(), true);
    EXPECT_EQ(mechanism.species[3].is_third_body.value(), true);

    EXPECT_EQ(mechanism.version.major, 1);
    EXPECT_EQ(mechanism.version.minor, 0);
    EXPECT_EQ(mechanism.version.patch, 0);
  }
}

TEST(ParserBase, ParserReportsBadFiles)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".yaml", ".json" };
  for (auto& extension : extensions)
  {
    std::string path = "examples/_missing_configuration" + extension;
    auto parsed = parser.Parse(path);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::FileNotFound);
  }
}

TEST(ParserBase, ParserReportsDirectory)
{
  v1::Parser parser;
  std::string path = "examples/";
  auto parsed = parser.Parse(path);
  EXPECT_FALSE(parsed);
  EXPECT_EQ(parsed.errors.size(), 1);
  EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::FileNotFound);
}

TEST(ParserBase, CanParseFromYamlString)
{
  v1::Parser parser;
  std::string content = R"(
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
    A: 32.1
    B: -2.3
    C: 102.3
    D: 63.4
    E: -1.3
    reactants:
      - species name: A
        coefficient: 1
    products:
      - species name: B
        coefficient: 1
)";

  auto parsed = parser.ParseFromString(content);
  EXPECT_TRUE(parsed);
  if (!parsed)
  {
    for (const auto& error : parsed.errors)
    {
      std::cerr << "Error: " << configParseStatusToString(error.first) << " - " << error.second << std::endl;
    }
  }
  v1::types::Mechanism mechanism = *parsed;
  EXPECT_EQ(mechanism.name, "Simple Configuration");
  EXPECT_EQ(mechanism.species.size(), 2);
  EXPECT_EQ(mechanism.phases.size(), 1);
  EXPECT_EQ(mechanism.reactions.arrhenius.size(), 1);
  EXPECT_EQ(mechanism.version.major, 1);
  EXPECT_EQ(mechanism.version.minor, 0);
  EXPECT_EQ(mechanism.version.patch, 0);
}

TEST(ParserBase, CanParseFromJsonString)
{
  v1::Parser parser;
  std::string content = R"({
  "version": "1.0.0",
  "name": "Simple Configuration",
  "species": [
    { "name": "A" },
    { "name": "B" }
  ],
  "phases": [
    {
      "name": "gas",
      "species": [ {"name": "A"}, {"name": "B"} ]
    }
  ],
  "reactions": [
    {
      "type": "ARRHENIUS",
      "name": "my arrhenius",
      "gas phase": "gas",
      "A": 32.1,
      "B": -2.3,
      "C": 102.3,
      "D": 63.4,
      "E": -1.3,
      "reactants": [
        {
          "species name": "A",
          "coefficient": 1
        }
      ],
      "products": [
        {
          "species name": "B",
          "coefficient": 1
        }
      ]
    }
  ]
})";

  auto parsed = parser.ParseFromString(content);
  EXPECT_TRUE(parsed);
  if (!parsed)
  {
    for (const auto& error : parsed.errors)
    {
      std::cerr << "Error: " << configParseStatusToString(error.first) << " - " << error.second << std::endl;
    }
  }
  v1::types::Mechanism mechanism = *parsed;
  EXPECT_EQ(mechanism.name, "Simple Configuration");
  EXPECT_EQ(mechanism.species.size(), 2);
  EXPECT_EQ(mechanism.phases.size(), 1);
  EXPECT_EQ(mechanism.reactions.arrhenius.size(), 1);
  EXPECT_EQ(mechanism.version.major, 1);
  EXPECT_EQ(mechanism.version.minor, 0);
  EXPECT_EQ(mechanism.version.patch, 0);
}

TEST(ParserBase, CanParseFromYamlNode)
{
  v1::Parser parser;
  std::string content = R"(
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
    A: 32.1
    B: -2.3
    C: 102.3
    D: 63.4
    E: -1.3
    reactants:
      - species name: A
        coefficient: 1
    products:
      - species name: B
        coefficient: 1
)";

  YAML::Node node = YAML::Load(content);

  auto parsed = parser.ParseFromNode(node);
  EXPECT_TRUE(parsed);
  v1::types::Mechanism mechanism = *parsed;
  EXPECT_EQ(mechanism.name, "Simple Configuration");
  EXPECT_EQ(mechanism.species.size(), 2);
  EXPECT_EQ(mechanism.phases.size(), 1);
  EXPECT_EQ(mechanism.reactions.arrhenius.size(), 1);
  EXPECT_EQ(mechanism.version.major, 1);
  EXPECT_EQ(mechanism.version.minor, 0);
  EXPECT_EQ(mechanism.version.patch, 0);
}
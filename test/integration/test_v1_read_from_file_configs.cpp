#include <mechanism_configuration/parse_status.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/types.hpp>

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include <string>
#include <vector>
#include <iostream>

using namespace mechanism_configuration;

TEST(ParserBase, ParsesFullV1Configuration)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { "/json/main.json", "/yaml/main.yaml" };
  for (auto& extension : extensions)
  {
    std::string path = "examples/v1/config" + extension;
    auto parsed = parser.Parse(path);
    for (const auto& error : parsed.errors)
      std::cout << "[" << configParseStatusToString(error.first) << "] " << error.second << "\n";
    ASSERT_TRUE(parsed);

    auto& mechanism = *parsed;

    EXPECT_EQ(mechanism.name, "troposphere and stratosphere configs");

    // species: A, B, C
    ASSERT_EQ(mechanism.species.size(), 3);
    EXPECT_EQ(mechanism.species[0].name, "A");
    EXPECT_EQ(mechanism.species[1].name, "B");
    EXPECT_EQ(mechanism.species[2].name, "C");

    // phases: one gas phase with 3 species
    ASSERT_EQ(mechanism.phases.size(), 1);
    EXPECT_EQ(mechanism.phases[0].name, "gas");
    ASSERT_EQ(mechanism.phases[0].species.size(), 3);

    // reactions: 3 from troposphere + 3 from stratosphere = 6 arrhenius
    EXPECT_EQ(mechanism.reactions.arrhenius.size(), 6);
  }
}
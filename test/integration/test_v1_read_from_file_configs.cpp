// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <string>

using namespace mechanism_configuration;

// A v1.1+ configuration may be split across multiple files via `{ files: [...] }`;
// Parse() resolves and merges them into one mechanism.
TEST(V1FileConfigs, ParsesMultiFileConfiguration)
{
  for (const auto& main : { std::string("examples/v1/config/json/main.json"),
                            std::string("examples/v1/config/yaml/main.yaml") })
  {
    auto parsed = Parse(main);
    if (!parsed)
      for (const auto& [code, message] : parsed.error())
        std::cout << "[" << ErrorCodeToString(code) << "] " << message << "\n";
    ASSERT_TRUE(parsed);

    const Mechanism& mechanism = *parsed;
    EXPECT_EQ(mechanism.name, "troposphere and stratosphere configs");

    ASSERT_EQ(mechanism.species.size(), 3);
    EXPECT_EQ(mechanism.species[0].name, "A");
    EXPECT_EQ(mechanism.species[1].name, "B");
    EXPECT_EQ(mechanism.species[2].name, "C");

    ASSERT_EQ(mechanism.phases.size(), 1);
    EXPECT_EQ(mechanism.phases[0].name, "gas");
    ASSERT_EQ(mechanism.phases[0].species.size(), 3);

    EXPECT_EQ(mechanism.reactions.arrhenius.size(), 6);
  }
}

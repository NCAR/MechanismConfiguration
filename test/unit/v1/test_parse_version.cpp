// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

using namespace mechanism_configuration;

namespace
{
  const std::vector<std::string> extensions = { ".json", ".yaml" };

  // Returns the messages of every error with the given code.
  std::vector<std::string> MessagesWithCode(const Errors& errors, ErrorCode code)
  {
    std::vector<std::string> messages;
    for (const auto& [error_code, message] : errors)
    {
      std::cout << message << " " << ErrorCodeToString(error_code) << std::endl;
      if (error_code == code)
        messages.push_back(message);
    }
    return messages;
  }

  bool Contains(const std::string& message, const std::string& text)
  {
    return message.find(text) != std::string::npos;
  }
}  // namespace

TEST(ParseVersion, AcceptsOldestAndNewestSupportedMinorVersions)
{
  for (const auto& extension : extensions)
  {
    for (const std::string name : { "minimal_1_0", "minimal_1_3" })
    {
      auto parsed = Parse("./v1_unit_configs/version/" + name + extension);
      EXPECT_TRUE(parsed) << name << extension;
    }
  }
}

TEST(ParseVersion, RejectsUnsupportedMinorVersion)
{
  for (const auto& extension : extensions)
  {
    auto parsed = Parse("./v1_unit_configs/version/unsupported_minor" + extension);
    ASSERT_FALSE(parsed);
    auto messages = MessagesWithCode(parsed.error(), ErrorCode::InvalidVersion);
    ASSERT_EQ(messages.size(), 1);
    EXPECT_TRUE(Contains(messages[0], "Version '1.4.0' is not supported"));
    EXPECT_TRUE(Contains(messages[0], "newest supported version is '1.3'"));
  }
}

TEST(ParseVersion, AerosolRequiresMinorVersionTwo)
{
  for (const auto& extension : extensions)
  {
    auto parsed = Parse("./v1_unit_configs/version/aerosol_1_1" + extension);
    ASSERT_FALSE(parsed);
    auto messages = MessagesWithCode(parsed.error(), ErrorCode::InvalidVersion);
    ASSERT_EQ(messages.size(), 2);
    EXPECT_TRUE(Contains(messages[0], "'aerosol representations' requires version '1.2' or newer"));
    EXPECT_TRUE(Contains(messages[1], "'aerosol processes' requires version '1.2' or newer"));

    parsed = Parse("./v1_unit_configs/version/aerosol_1_2" + extension);
    EXPECT_TRUE(parsed);
    ASSERT_TRUE(parsed->aerosol.has_value());
    EXPECT_EQ(parsed->aerosol->representations.size(), 1);
    EXPECT_EQ(parsed->aerosol->processes.size(), 1);
  }
}

TEST(ParseVersion, EmissionsRequiresMinorVersionThree)
{
  for (const auto& extension : extensions)
  {
    auto parsed = Parse("./v1_unit_configs/version/emissions_1_2" + extension);
    ASSERT_FALSE(parsed);
    auto messages = MessagesWithCode(parsed.error(), ErrorCode::InvalidVersion);
    ASSERT_EQ(messages.size(), 1);
    EXPECT_TRUE(Contains(messages[0], "'emissions' requires version '1.3' or newer"));

    parsed = Parse("./v1_unit_configs/version/emissions_1_3" + extension);
    EXPECT_TRUE(parsed);
    ASSERT_TRUE(parsed->emissions.has_value());
    EXPECT_EQ(parsed->emissions->sources.size(), 1);
  }
}

TEST(ParseVersion, ParseFromStringChecksTheMinorVersion)
{
  const std::string config = R"(
version: 1.9.0
species:
  - name: A
phases:
  - name: gas
    species:
      - name: A
reactions: []
)";
  auto parsed = ParseFromString(config);
  ASSERT_FALSE(parsed);
  auto messages = MessagesWithCode(parsed.error(), ErrorCode::InvalidVersion);
  ASSERT_EQ(messages.size(), 1);
  EXPECT_TRUE(Contains(messages[0], "Version '1.9.0' is not supported"));
}

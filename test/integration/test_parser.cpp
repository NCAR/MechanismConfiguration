#include <mechanism_configuration/parser.hpp>
#include <mechanism_configuration/v1/types.hpp>

#include <gtest/gtest.h>

TEST(UniversalParser, ConfigurationWithoutVersionFallsbackToV0)
{
  mechanism_configuration::UniversalParser parser;
  std::vector<std::string> extensions = { ".yaml", ".json" };
  for (auto& extension : extensions)
  {
    std::string path = "examples/v0/config" + extension;
    auto parsed = parser.Parse(path);
    EXPECT_TRUE(parsed);

    EXPECT_EQ(parsed.mechanism->version.major, 0);
    EXPECT_EQ(parsed.mechanism->version.minor, 0);
    EXPECT_EQ(parsed.mechanism->version.patch, 0);
  }
}

TEST(UniversalParser, ParsesFullV1Configuration)
{
  mechanism_configuration::UniversalParser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string path = "examples/v1/full_configuration" + extension;
    auto parsed = parser.Parse(path);
    if (!parsed)
    {
      std::cout << "Errors: " << std::endl;
      for (const auto& error : parsed.errors)
      {
        std::cout << error.second << std::endl;
      }
    }
    EXPECT_TRUE(parsed);

    EXPECT_EQ(parsed.mechanism->version.major, 1);
    EXPECT_EQ(parsed.mechanism->version.minor, 0);
    EXPECT_EQ(parsed.mechanism->version.patch, 0);
  }
}

TEST(UniversalParser, ParsesFullDevConfiguration)
{
  mechanism_configuration::UniversalParser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string path = "examples/development/full_configuration" + extension;
    auto parsed = parser.Parse(path);
    if (!parsed)
    {
      std::cout << "Errors: " << std::endl;
      for (const auto& error : parsed.errors)
      {
        std::cout << error.second << std::endl;
      }
    }
    EXPECT_TRUE(parsed);

    EXPECT_EQ(parsed.mechanism->version.major, 2);
    EXPECT_EQ(parsed.mechanism->version.minor, 0);
    EXPECT_EQ(parsed.mechanism->version.patch, 0);
  }
}

TEST(UniversalParser, ParserReportsBadFiles)
{
  mechanism_configuration::UniversalParser parser;
  std::vector<std::string> extensions = { ".yaml", ".json" };
  for (auto& extension : extensions)
  {
    std::string path = "examples/_missing_configuration" + extension;
    auto parsed = parser.Parse(path);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, mechanism_configuration::ConfigParseStatus::FileNotFound);
  }
}

TEST(UniversalParser, ParseUnsupportedVersion)
{
  using namespace mechanism_configuration;

  UniversalParser parser;
  auto parsed = parser.Parse("integration_configs/invalid_version.yaml");
  EXPECT_EQ(parsed.errors.size(), 1);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::InvalidVersion };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : parsed.errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}
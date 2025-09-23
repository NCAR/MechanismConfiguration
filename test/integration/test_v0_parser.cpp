#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/types.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, ParsesFullv0Configuration)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".yaml", ".json" };
  for (auto& extension : extensions)
  {
    std::string path = "examples/v0/config" + extension;
    auto parsed = parser.Parse(path);
    EXPECT_TRUE(parsed);

    v0::types::Mechanism mechanism = *parsed;
    EXPECT_EQ(mechanism.reactions.user_defined.size(), 4);
    EXPECT_EQ(mechanism.reactions.arrhenius.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe.size(), 1);
    EXPECT_EQ(mechanism.reactions.ternary_chemical_activation.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched.size(), 2);
    EXPECT_EQ(mechanism.reactions.tunneling.size(), 2);
    EXPECT_EQ(mechanism.reactions.surface.size(), 1);

    EXPECT_EQ(mechanism.version.major, 0);
    EXPECT_EQ(mechanism.version.minor, 0);
    EXPECT_EQ(mechanism.version.patch, 0);
  }
}

TEST(ParserBase, ParsesSurface)
{
  v0::Parser parser;
  std::string path = "examples/v0/surface/config.json";
  auto parsed = parser.Parse(path);
  EXPECT_TRUE(parsed);

  for (auto& error : parsed.errors)
  {
    std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
  }

  v0::types::Mechanism mechanism = *parsed;
  EXPECT_EQ(mechanism.reactions.surface.size(), 1);
}

TEST(ParserBase, ParserReportsBadFiles)
{
  v0::Parser parser;
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

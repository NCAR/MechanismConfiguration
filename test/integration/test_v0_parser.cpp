#include <gtest/gtest.h>

#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/types.hpp>

using namespace mechanism_configuration;

TEST(ParserBase, ParsesFullv0Configuration)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { "json"};
  for (auto& extension : extensions)
  {
    std::string path = "examples/v0/" + extension;
    auto parsed = parser.Parse(path);
    EXPECT_TRUE(parsed);
    v0::types::Mechanism mechanism = *parsed;
    EXPECT_EQ(mechanism.reactions.user_defined.size(), 3);
    EXPECT_EQ(mechanism.reactions.arrhenius.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe.size(), 1);
    EXPECT_EQ(mechanism.reactions.ternary_chemical_activation.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched.size(), 2);
    EXPECT_EQ(mechanism.reactions.tunneling.size(), 2);
    EXPECT_EQ(mechanism.reactions.surface.size(), 1);
  }
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
  }
}

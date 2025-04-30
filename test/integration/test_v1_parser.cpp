#include <gtest/gtest.h>

#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/types.hpp>

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
    EXPECT_EQ(mechanism.species.size(), 11);
    EXPECT_EQ(mechanism.phases.size(), 4);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium.size(), 1);
    EXPECT_EQ(mechanism.reactions.arrhenius.size(), 2);
    EXPECT_EQ(mechanism.reactions.branched.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius.size(), 2);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis.size(), 1);
    EXPECT_EQ(mechanism.reactions.emission.size(), 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis.size(), 1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer.size(), 1);
    EXPECT_EQ(mechanism.reactions.surface.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined.size(), 1);

    EXPECT_EQ(mechanism.species[1].tracer_type.has_value(), true);
    EXPECT_EQ(mechanism.species[1].tracer_type.value(), "AEROSOL");
    EXPECT_EQ(mechanism.species[2].tracer_type.has_value(), true);
    EXPECT_EQ(mechanism.species[2].tracer_type.value(), "THIRD_BODY");

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

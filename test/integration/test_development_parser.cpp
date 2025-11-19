#include <mechanism_configuration/development/mechanism_parser.hpp>
#include <mechanism_configuration/development/types.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParseDevFullConfig, ParseValidConfig)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string path = "examples/development/full_configuration" + extension;
    auto parsed = parser.Parse(path);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;
    EXPECT_EQ(mechanism.name, "Full Configuration");
    EXPECT_EQ(mechanism.species.size(), 11);
    EXPECT_EQ(mechanism.phases.size(), 4);

    EXPECT_EQ(mechanism.models.gas_model.type, "GAS_PHASE");
    EXPECT_EQ(mechanism.models.modal_model.modes.size(), 2);

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
    EXPECT_EQ(mechanism.reactions.taylor_series.size(), 1);
    EXPECT_EQ(mechanism.reactions.ternary_chemical_activation.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling.size(), 1);
    EXPECT_EQ(mechanism.reactions.user_defined.size(), 1);
    EXPECT_EQ(mechanism.reactions.wet_deposition.size(), 1);

    EXPECT_EQ(mechanism.species[1].constant_concentration.has_value(), true);
    EXPECT_EQ(mechanism.species[1].constant_concentration.value(), 1.0e19);
    EXPECT_EQ(mechanism.species[2].is_third_body.has_value(), true);
    EXPECT_EQ(mechanism.species[2].is_third_body.value(), true);
    EXPECT_EQ(mechanism.species[4].constant_mixing_ratio.has_value(), true);
    EXPECT_EQ(mechanism.species[4].constant_mixing_ratio.value(), 1.0e-6);

    EXPECT_EQ(mechanism.version.major, 2);
    EXPECT_EQ(mechanism.version.minor, 0);
    EXPECT_EQ(mechanism.version.patch, 0);
  }
}

TEST(ParseDevFullConfig, ReportsBadFiles)
{
  development::Parser parser;
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

TEST(ParseDevFullConfig, ReportsDirectory)
{
  development::Parser parser;
  std::string path = "examples/";
  auto parsed = parser.Parse(path);
  EXPECT_FALSE(parsed);
  EXPECT_EQ(parsed.errors.size(), 1);
  EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::FileNotFound);
}

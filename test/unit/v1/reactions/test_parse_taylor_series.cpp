#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidTaylorSeriesReaction)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parse(std::string("v1_unit_configs/reactions/taylor_series/valid") + extension);
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.taylor_series.size(), 3);

    EXPECT_EQ(mechanism.reactions.taylor_series[0].name, "my taylor_series");
    EXPECT_EQ(mechanism.reactions.taylor_series[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.taylor_series[0].A, 32.1);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].B, -2.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].C, 102.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].D, 63.4);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].E, -1.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].taylor_coefficients.size(), 3);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].taylor_coefficients[0], 1.0);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].taylor_coefficients[1], 2.0);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].taylor_coefficients[2], 3.0);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.taylor_series[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].products[0].name, "B");
    EXPECT_EQ(mechanism.reactions.taylor_series[0].products[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].products[1].name, "C");
    EXPECT_EQ(mechanism.reactions.taylor_series[0].products[1].coefficient, 0.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[0].unknown_properties["__solver_param"], "0.1");

    EXPECT_EQ(mechanism.reactions.taylor_series[1].name, "my taylor_series2");
    EXPECT_EQ(mechanism.reactions.taylor_series[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.taylor_series[1].A, 3.1);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].B, -0.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].C, 12.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].D, 6.4);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].E, -0.3);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].taylor_coefficients.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].taylor_coefficients[0], 10.5);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].reactants.size(), 2);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.taylor_series[1].reactants[0].coefficient, 2);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].reactants[1].name, "B");
    EXPECT_EQ(mechanism.reactions.taylor_series[1].reactants[1].coefficient, 0.1);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.taylor_series[1].products[0].coefficient, 0.5);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[1].products[0].unknown_properties["__optional thing"], "hello");

    EXPECT_EQ(mechanism.reactions.taylor_series[2].name, "");
    EXPECT_EQ(mechanism.reactions.taylor_series[2].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.taylor_series[2].A, 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].B, 0);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].C, 0);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].D, 300);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].E, 0);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].taylor_coefficients.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].taylor_coefficients[0], 1.0);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.taylor_series[2].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.taylor_series[2].products[0].name, "C");
    EXPECT_EQ(mechanism.reactions.taylor_series[2].products[0].coefficient, 1);
  }
}

TEST(ParserBase, TaylorSeriesDetectsUnknownSpecies)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/taylor_series/unknown_species") + extension;
    auto parsed = parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 2);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::ReactionRequiresUnknownSpecies);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, TaylorSeriesDetectsMutuallyExclusiveOptions)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/taylor_series/mutually_exclusive") + extension;
    auto parsed = parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::MutuallyExclusiveOption);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, TaylorSeriesDetectsBadReactionComponent)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/taylor_series/bad_reaction_component") + extension;
    auto parsed = parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 2);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::RequiredKeyNotFound);
    EXPECT_EQ(parsed.error()[1].first, ErrorCode::InvalidKey);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, TaylorSeriesDetectsUnknownPhase)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/taylor_series/missing_phase") + extension;
    auto parsed = parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::UnknownPhase);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

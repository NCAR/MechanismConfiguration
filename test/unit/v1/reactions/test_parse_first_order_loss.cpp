#include <mechanism_configuration/v1/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidFirstOrderLossReaction)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/first_order_loss/valid") + extension);
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.first_order_loss.size(), 2);

    EXPECT_EQ(mechanism.reactions.first_order_loss[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].name, "my first order loss");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].reactants.name, "C");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].reactants.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].unknown_properties.size(), 1);
    EXPECT_EQ(
        mechanism.reactions.first_order_loss[0].unknown_properties["__comment"], "Strawberries are the superior fruit");

    EXPECT_EQ(mechanism.reactions.first_order_loss[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].reactants.name, "C");
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].reactants.coefficient, 1);
  }
}

TEST(ParserBase, FirstOrderLossDetectsUnknownSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/first_order_loss/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::ReactionRequiresUnknownSpecies);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, FirstOrderLossDetectsBadReactionComponent)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/first_order_loss/bad_reaction_component") + extension;
    auto parsed = parser.Parse(file);
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

TEST(ParserBase, FirstOrderLossDetectsUnknownPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/first_order_loss/missing_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::UnknownPhase);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, FirstOrderLossDetectsMoreThanOneSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/first_order_loss/too_many_reactants") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::TooManyReactionComponents);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, CanParseValidFirstOrderLossReactionWithProducts)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/first_order_loss/products") + extension);
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.first_order_loss.size(), 2);

    EXPECT_EQ(mechanism.reactions.first_order_loss[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].name, "my first order loss");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].reactants.name, "C");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].reactants.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].products[0].name, "A");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].products[1].name, "B");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].products[1].coefficient, 2);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].unknown_properties.size(), 1);
    EXPECT_EQ(
        mechanism.reactions.first_order_loss[0].unknown_properties["__comment"], "Strawberries are the superior fruit");

    EXPECT_EQ(mechanism.reactions.first_order_loss[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].reactants.name, "C");
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].reactants.coefficient, 1);
  }
}
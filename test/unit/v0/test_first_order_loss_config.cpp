#include <gtest/gtest.h>

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v0/parser.hpp>

using namespace mechanism_configuration;

TEST(FirstOrderLossConfig, DetectsInvalidConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/first_order_loss/missing_reactants/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/first_order_loss/missing_MUSICA_name/config" + extension;
    parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(FirstOrderLossConfig, ParseConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/first_order_loss/valid/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_TRUE(parsed);
    v0::types::Mechanism mechanism = *parsed;

    auto& process_vector = mechanism.reactions.user_defined;
    EXPECT_EQ(process_vector.size(), 2);

    // first reaction
    {
      EXPECT_EQ(process_vector[0].reactants.size(), 1);
      EXPECT_EQ(process_vector[0].products.size(), 0);
      EXPECT_EQ(process_vector[0].reactants[0].species_name, "foo");
      EXPECT_EQ(process_vector[0].name, "LOSS.foo");
      EXPECT_EQ(process_vector[0].scaling_factor, 1.0);
    }

    // second reaction
    {
      EXPECT_EQ(process_vector[1].reactants.size(), 1);
      EXPECT_EQ(process_vector[1].products.size(), 0);
      EXPECT_EQ(process_vector[1].reactants[0].species_name, "bar");
      EXPECT_EQ(process_vector[1].name, "LOSS.bar");
      EXPECT_EQ(process_vector[1].scaling_factor, 2.5);
    }
  }
}

TEST(FirstOrderLossConfig, DetectsNonstandardKeys)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/first_order_loss/contains_nonstandard_key/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::InvalidKey);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}
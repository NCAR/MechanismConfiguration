#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidSurfaceReaction)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = Parse(std::string("v1_unit_configs/reactions/surface/valid") + extension);
    EXPECT_TRUE(parsed);
    Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.surface.size(), 2);

    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.surface[0].name, "my surface");
    EXPECT_EQ(mechanism.reactions.surface[0].reaction_probability, 2.0e-2);
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_species.name, "A");
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_species.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products.size(), 2);
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products[0].name, "B");
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products[1].name, "C");
    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products[1].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.surface[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.surface[0].unknown_properties["__comment"], "key lime pie is superior to all other pies");

    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.surface[1].reaction_probability, 1.0);
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_species.name, "A");
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_species.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products.size(), 2);
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[0].name, "B");
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[0].unknown_properties["__optional thing"], "hello");
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[1].name, "C");
    EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[1].coefficient, 1);
  }
}

TEST(ParserBase, SurfaceDetectsUnknownSpecies)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/surface/unknown_species") + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::ReactionRequiresUnknownSpecies);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, SurfaceDetectsBadReactionComponent)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/surface/bad_reaction_component") + extension;
    auto parsed = Parse(file);
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

TEST(ParserBase, SurfaceDetectsUnknownGasPhase)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("v1_unit_configs/reactions/surface/missing_gas_phase") + extension;
    auto parsed = Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.error().size(), 1);
    EXPECT_EQ(parsed.error()[0].first, ErrorCode::UnknownPhase);
    for (auto& error : parsed.error())
    {
      std::cout << error.second << " " << ErrorCodeToString(error.first) << std::endl;
    }
  }
}

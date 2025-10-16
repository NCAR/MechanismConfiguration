#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidEmissionReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/emission/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.emission.size(), 2);

    EXPECT_EQ(mechanism.reactions.emission[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.emission[0].name, "my emission");
    EXPECT_EQ(mechanism.reactions.emission[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.emission[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.emission[0].products[0].name, "B");
    EXPECT_EQ(mechanism.reactions.emission[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.emission[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.emission[0].unknown_properties["__comment"], "Dr. Pepper outranks any other soda");

    EXPECT_EQ(mechanism.reactions.emission[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.emission[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.emission[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.emission[1].products[0].name, "B");
    EXPECT_EQ(mechanism.reactions.emission[1].products[0].coefficient, 1);
  }
}

TEST(ParserBase, EmissionDetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/emission/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, EmissionDetectsBadReactionComponent)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/emission/bad_reaction_component") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    EXPECT_EQ(parsed.errors[1].first, ConfigParseStatus::InvalidKey);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, EmissionDetectsUnknownPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/emission/missing_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::UnknownPhase);
    for (auto& error : parsed.errors)
    {
      std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

TEST(ParserBase, EmissionUnknownSpeciesAndUnknownPhaseFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { 
    types::Species{ .name = "foo" }, 
    types::Species{ .name = "bar" }
  };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "EMISSION";
  reaction_node["products"] = YAML::Load("[{ name: quiz }]");

  // Unknown gas phase name triggers validation error
  reaction_node["gas phase"] = "what is emission phase";

  EmissionParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  ASSERT_FALSE(errors.empty());
  EXPECT_EQ(errors.size(), 2);
  EXPECT_EQ(errors[0].first, ConfigParseStatus::ReactionRequiresUnknownSpecies);
  EXPECT_EQ(errors[1].first, ConfigParseStatus::UnknownPhase);
  for (auto& error : errors)
  {
    std::cout << error.second << " " << configParseStatusToString(error.first) << std::endl;
  }
}
#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidSurfaceReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/surface/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.surface.size(), 2);

    EXPECT_EQ(mechanism.reactions.surface[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.surface[0].name, "my surface");
    EXPECT_EQ(mechanism.reactions.surface[0].condensed_phase, "surface reacting phase");
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
    EXPECT_EQ(mechanism.reactions.surface[1].condensed_phase, "surface reacting phase");
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
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/surface/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::ReactionRequiresUnknownSpecies,
                                                  ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, SurfaceDetectsBadReactionComponent)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/surface/bad_reaction_component") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 2);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound, ConfigParseStatus::InvalidKey };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, SurfaceDetectsUnknownCondensedPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/surface/missing_condensed_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::UnknownPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, SurfaceDetectsUnknownGasPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/surface/missing_gas_phase") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::UnknownPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, SurfaceInvalidNumberReactantUnknownSpeciesUnknownPhaseFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" }, types::Species{ .name = "bar" } };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "SURFACE";
  reaction_node["reaction probability"] = 0.7;
  reaction_node["gas-phase products"] = YAML::Load("[{ name: foo }]");
  reaction_node["gas phase"] = "gas";

  // Invalid number of reactions triggers validation error
  reaction_node["gas-phase species"] = YAML::Load("[{ name: quiz }, { name: bar }]");

  // Unknown condensed phase name triggers validation error
  reaction_node["condensed phase"] = "condensed phase";

  SurfaceParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 3);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::TooManyReactionComponents,
                                                ConfigParseStatus::ReactionRequiresUnknownSpecies,
                                                ConfigParseStatus::UnknownPhase };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}
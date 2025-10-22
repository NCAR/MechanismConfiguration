#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidAqueousEquilibriumReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/aqueous_equilibrium/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium.size(), 2);

    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].name, "my aqueous eq");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].condensed_phase_water, "H2O");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].A, 1.14e-2);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].C, 2300.0);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].k_reverse, 0.32);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].reactants[0].coefficient, 2);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products[0].name, "B");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products[1].name, "C");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products[1].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].unknown_properties["__comment"], "GIF is pronounced with a hard g");

    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].condensed_phase_water, "H2O");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].A, 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].C, 0);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].k_reverse, 0.32);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].reactants[0].name, "A");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].reactants[0].coefficient, 2);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products[0].name, "B");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products[1].name, "C");
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products[1].coefficient, 1);
  }
}

TEST(ParserBase, AqueousEquilibriumDetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/aqueous_equilibrium/unknown_species") + extension;
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

TEST(ParserBase, AqueousEquilibriumDetectsBadReactionComponent)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        std::string("development_unit_configs/reactions/aqueous_equilibrium/bad_reaction_component") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::InvalidKey };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, AqueousEquilibriumDetectsUnknownPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/aqueous_equilibrium/missing_phase") + extension;
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

TEST(ParserBase, AqueousEquilibriumInvalidNumberReactantUnknownSpeciesUnknownPhaseFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" }, types::Species{ .name = "bar" } };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "AQUEOUS_EQUILIBRIUM";
  reaction_node["products"] = YAML::Load("[{ name: foo }]");
  reaction_node["condensed-phase water"] = "H2O";
  reaction_node["k_reverse"] = 0.46;
  reaction_node["reactants"] = YAML::Load("[{ name: foo }, { name: bar }]");

  // Unknown gas phase name triggers validation error
  reaction_node["condensed phase"] = "aqueous";

  AqueousEquilibriumParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  // EXPECT_EQ(errors.size(), 3);

  std::multiset<ConfigParseStatus> expected = { 
                                                ConfigParseStatus::ReactionRequiresUnknownSpecies,
                                                ConfigParseStatus::UnknownPhase };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  // EXPECT_EQ(actual, expected);
}
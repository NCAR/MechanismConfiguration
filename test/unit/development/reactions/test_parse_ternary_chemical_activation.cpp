#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParserBase, TernaryParseValidConfig)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };

  for (auto& extension : extensions)
  {
    std::string file = "./development_unit_configs/reactions/ternary_chemical_activation/valid" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    auto& process_vector = mechanism.reactions.ternary_chemical_activation;
    EXPECT_EQ(process_vector.size(), 2);

    // first reaction
    EXPECT_EQ(process_vector[0].reactants.size(), 2);
    EXPECT_EQ(process_vector[0].reactants[0].name, "foo");
    EXPECT_EQ(process_vector[0].reactants[0].coefficient, 1.0);
    EXPECT_EQ(process_vector[0].reactants[1].name, "quz");
    EXPECT_EQ(process_vector[0].reactants[1].coefficient, 2.0);
    EXPECT_EQ(process_vector[0].products.size(), 2);
    EXPECT_EQ(process_vector[0].products[0].name, "bar");
    EXPECT_EQ(process_vector[0].products[0].coefficient, 1.0);
    EXPECT_EQ(process_vector[0].products[1].name, "baz");
    EXPECT_EQ(process_vector[0].products[1].coefficient, 3.2);
    EXPECT_EQ(process_vector[0].k0_A, 1.0);
    EXPECT_EQ(process_vector[0].k0_B, 0.0);
    EXPECT_EQ(process_vector[0].k0_C, 0.0);
    EXPECT_EQ(process_vector[0].kinf_A, 1.0);
    EXPECT_EQ(process_vector[0].kinf_B, 0.0);
    EXPECT_EQ(process_vector[0].kinf_C, 0.0);
    EXPECT_EQ(process_vector[0].Fc, 0.6);
    EXPECT_EQ(process_vector[0].N, 1.0);

    // second reaction
    EXPECT_EQ(process_vector[1].unknown_properties.size(), 1);
    EXPECT_EQ(process_vector[1].unknown_properties["__optional thing"], "hello");
    EXPECT_EQ(process_vector[1].reactants.size(), 2);
    EXPECT_EQ(process_vector[1].reactants[0].name, "bar");
    EXPECT_EQ(process_vector[1].reactants[1].name, "baz");
    EXPECT_EQ(process_vector[1].products.size(), 2);
    EXPECT_EQ(process_vector[1].products[0].name, "bar");
    EXPECT_EQ(process_vector[1].products[0].coefficient, 0.5);
    EXPECT_EQ(process_vector[1].products[1].name, "foo");
    EXPECT_EQ(process_vector[1].products[1].coefficient, 0.0);
    EXPECT_EQ(process_vector[1].k0_A, 32.1);
    EXPECT_EQ(process_vector[1].k0_B, -2.3);
    EXPECT_EQ(process_vector[1].k0_C, 102.3);
    EXPECT_EQ(process_vector[1].kinf_A, 63.4);
    EXPECT_EQ(process_vector[1].kinf_B, -1.3);
    EXPECT_EQ(process_vector[1].kinf_C, 908.5);
    EXPECT_EQ(process_vector[1].Fc, 1.3);
    EXPECT_EQ(process_vector[1].N, 32.1);
    EXPECT_EQ(process_vector[1].name, "my ternary chemical activation");
  }
}

TEST(ParserBase, TernaryDetectsNonStandardKey)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        "./development_unit_configs/reactions/ternary_chemical_activation/contains_nonstandard_key" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 12);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound, ConfigParseStatus::InvalidKey,
                                                  ConfigParseStatus::RequiredKeyNotFound, ConfigParseStatus::InvalidKey,
                                                  ConfigParseStatus::InvalidKey,          ConfigParseStatus::InvalidKey,
                                                  ConfigParseStatus::InvalidKey,          ConfigParseStatus::InvalidKey,
                                                  ConfigParseStatus::InvalidKey,          ConfigParseStatus::InvalidKey,
                                                  ConfigParseStatus::InvalidKey,          ConfigParseStatus::InvalidKey };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, TernaryDetectsMissingProducts)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./development_unit_configs/reactions/ternary_chemical_activation/missing_products" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, TernaryDetectsMissingReactants)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./development_unit_configs/reactions/ternary_chemical_activation/missing_reactants" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, TernaryUnknownSpeciesAndUnknownPhaseFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "foo" },
                                                   types::Species{ .name = "bar" },
                                                   types::Species{ .name = "quiz" } };

  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "TERNARY_CHEMICAL_ACTIVATION";
  reaction_node["products"] = YAML::Load("[{ name: quiz }]");

  // Unknown species triggers validation error
  reaction_node["reactants"] = YAML::Load("[{ name: bar }, { name: ABC }]");

  // Unknown gas phase name triggers validation error
  reaction_node["gas phase"] = "Gaseous Phase";

  TernaryChemicalActivationParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 2);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::ReactionRequiresUnknownSpecies,
                                                ConfigParseStatus::UnknownPhase };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}
#include <mechanism_configuration/development/parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidSimpolPhaseTransferReaction)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/simpol_phase_transfer/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer.size(), 2);

    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].name, "my simpol");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].gas_phase_species[0].name, "A");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].gas_phase_species[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].condensed_phase_species[0].name, "B");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].condensed_phase_species[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].B[0], -1.97e3);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].B[1], 2.91e0);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].B[2], 1.96e-3);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].B[3], -4.96e-1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].unknown_properties["__comment"], "cereal is also soup");

    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].name, "");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].gas_phase_species[0].name, "A");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].gas_phase_species[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].condensed_phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].condensed_phase_species[0].name, "B");
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].condensed_phase_species[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].B[0], -1.97e3);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].B[1], 2.91e0);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].B[2], 1.96e-3);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].B[3], -4.96e-1);
  }
}

TEST(ParserBase, SimpolPhaseTransferDetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/simpol_phase_transfer/unknown_species") + extension;
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

TEST(ParserBase, SimpolPhaseTransferDetectsUnknownAqueousPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        std::string("development_unit_configs/reactions/simpol_phase_transfer/missing_aqueous_phase") + extension;
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

TEST(ParserBase, SimpolPhaseTransferDetectsUnknownGasPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/simpol_phase_transfer/missing_gas_phase") + extension;
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

TEST(ParserBase, SimpolPhaseTransferDetectsUnknownGasPhaseSpeciesNotInGasPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        std::string("development_unit_configs/reactions/simpol_phase_transfer/missing_gas_phase_species_in_gas_phase") +
        extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, SimpolPhaseTransferDetectsUnknownAqueousPhaseSpeciesNotInAqueousPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        std::string(
            "development_unit_configs/reactions/simpol_phase_transfer/missing_aqueous_phase_species_in_aqueous_phase") +
        extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase};
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParserBase, SimpolPhaseTransferInvalidBParameterNotSequenceFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "A" }, types::Species{ .name = "B" } };
  std::vector<types::Phase> existing_phases = { 
    types::Phase{ .name = "gas", .species = { types::PhaseSpecies{ .name = "A" } } },
    types::Phase{ .name = "aqueous", .species = { types::PhaseSpecies{ .name = "B" } } }
  };

  YAML::Node reaction_node;
  reaction_node["type"] = "SIMPOL_PHASE_TRANSFER";
  reaction_node["gas phase"] = "gas";
  reaction_node["gas-phase species"] = YAML::Load("[{ name: A, coefficient: 1 }]");
  reaction_node["condensed phase"] = "aqueous";
  reaction_node["condensed-phase species"] = YAML::Load("[{ name: B, coefficient: 1 }]");
  
  // Invalid B parameter - not a sequence
  reaction_node["B"] = "not a sequence";

  SimpolPhaseTransferParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 1);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::InvalidParameterNumber };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

TEST(ParserBase, SimpolPhaseTransferInvalidBParameterWrongCountFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "A" }, types::Species{ .name = "B" } };
  std::vector<types::Phase> existing_phases = { 
    types::Phase{ .name = "gas", .species = { types::PhaseSpecies{ .name = "A" } } },
    types::Phase{ .name = "aqueous", .species = { types::PhaseSpecies{ .name = "B" } } }
  };

  YAML::Node reaction_node;
  reaction_node["type"] = "SIMPOL_PHASE_TRANSFER";
  reaction_node["gas phase"] = "gas";
  reaction_node["gas-phase species"] = YAML::Load("[{ name: A, coefficient: 1 }]");
  reaction_node["condensed phase"] = "aqueous";
  reaction_node["condensed-phase species"] = YAML::Load("[{ name: B, coefficient: 1 }]");
  
  // Invalid B parameter - wrong number of parameters (should be 4)
  reaction_node["B"] = YAML::Load("[-1.97E+03, 2.91E+00]");

  SimpolPhaseTransferParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 1);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::InvalidParameterNumber };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

TEST(ParserBase, SimpolPhaseTransferTooManyGasSpeciesFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { 
    types::Species{ .name = "A" }, 
    types::Species{ .name = "B" }, 
    types::Species{ .name = "C" } 
  };
  std::vector<types::Phase> existing_phases = { 
    types::Phase{ .name = "gas", .species = { 
      types::PhaseSpecies{ .name = "A" }, 
      types::PhaseSpecies{ .name = "C" } 
    } },
    types::Phase{ .name = "aqueous", .species = { types::PhaseSpecies{ .name = "B" } } }
  };

  YAML::Node reaction_node;
  reaction_node["type"] = "SIMPOL_PHASE_TRANSFER";
  reaction_node["gas phase"] = "gas";
  
  // Too many gas phase species (should be exactly 1)
  reaction_node["gas-phase species"] = YAML::Load("[{ name: A, coefficient: 1 }, { name: C, coefficient: 1 }]");
  
  reaction_node["condensed phase"] = "aqueous";
  reaction_node["condensed-phase species"] = YAML::Load("[{ name: B, coefficient: 1 }]");
  reaction_node["B"] = YAML::Load("[-1.97E+03, 2.91E+00, 1.96E-03, -4.96E-01]");

  SimpolPhaseTransferParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 1);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::TooManyReactionComponents };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

TEST(ParserBase, SimpolPhaseTransferTooManyCondensedSpeciesFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { 
    types::Species{ .name = "A" }, 
    types::Species{ .name = "B" }, 
    types::Species{ .name = "C" } 
  };
  std::vector<types::Phase> existing_phases = { 
    types::Phase{ .name = "gas", .species = { types::PhaseSpecies{ .name = "A" } } },
    types::Phase{ .name = "aqueous", .species = { 
      types::PhaseSpecies{ .name = "B" }, 
      types::PhaseSpecies{ .name = "C" } 
    } }
  };

  YAML::Node reaction_node;
  reaction_node["type"] = "SIMPOL_PHASE_TRANSFER";
  reaction_node["gas phase"] = "gas";
  reaction_node["gas-phase species"] = YAML::Load("[{ name: A, coefficient: 1 }]");
  reaction_node["condensed phase"] = "aqueous";
  
  // Too many condensed phase species (should be exactly 1)
  reaction_node["condensed-phase species"] = YAML::Load("[{ name: B, coefficient: 1 }, { name: C, coefficient: 1 }]");
  
  reaction_node["B"] = YAML::Load("[-1.97E+03, 2.91E+00, 1.96E-03, -4.96E-01]");

  SimpolPhaseTransferParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 1);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::TooManyReactionComponents };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

TEST(ParserBase, SimpolPhaseTransferMultipleErrorsFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "A" }, types::Species{ .name = "B" } };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" } }; // Missing aqueous phase

  YAML::Node reaction_node;
  reaction_node["type"] = "SIMPOL_PHASE_TRANSFER";
  reaction_node["gas phase"] = "gas";
  
  // Too many gas species + unknown species + unknown phase + invalid B parameter
  reaction_node["gas-phase species"] = YAML::Load("[{ name: A, coefficient: 1 }, { name: UNKNOWN, coefficient: 1 }]");
  reaction_node["condensed phase"] = "aqueous"; // Unknown phase
  reaction_node["condensed-phase species"] = YAML::Load("[{ name: B, coefficient: 1 }]");
  reaction_node["B"] = "invalid"; // Invalid B parameter

  SimpolPhaseTransferParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_GE(errors.size(), 3);

  std::multiset<ConfigParseStatus> expected = { 
    ConfigParseStatus::TooManyReactionComponents,
    ConfigParseStatus::ReactionRequiresUnknownSpecies,
    ConfigParseStatus::UnknownPhase,
    ConfigParseStatus::InvalidParameterNumber
  };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}
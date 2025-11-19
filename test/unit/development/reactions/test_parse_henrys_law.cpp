#include <mechanism_configuration/development/mechanism_parser.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>

#include <gtest/gtest.h>

#include <set>

using namespace mechanism_configuration;

TEST(ParseHenrysLaw, ParseValidConfig)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("development_unit_configs/reactions/henrys_law/valid") + extension);
    EXPECT_TRUE(parsed);
    development::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.henrys_law.size(), 2);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].name, "my henry's law");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].gas.name, "gas");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].gas.species.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].gas.species[0].name, "A");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solutes.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solutes[0].name, "B");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solutes[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solvent.name, "H2O");
    EXPECT_EQ(mechanism.reactions.henrys_law[0].particle.solvent.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[0].unknown_properties["__comment"], "B condensed phase production (kg/m2/s)");

    EXPECT_EQ(mechanism.reactions.henrys_law[1].name, "");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].gas.name, "gas");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].gas.species.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].gas.species[0].name, "A");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.phase, "aqueous");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes.size(), 2);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes[0].name, "B");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes[1].name, "C");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solutes[1].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solvent.name, "H2O");
    EXPECT_EQ(mechanism.reactions.henrys_law[1].particle.solvent.coefficient, 1);
    EXPECT_EQ(mechanism.reactions.henrys_law[1].unknown_properties.size(), 0);
  }
}

TEST(ParseHenrysLaw, DetectsUnknownSpecies)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/henrys_law/unknown_species") + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);

    std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::PhaseRequiresUnknownSpecies };
    std::multiset<ConfigParseStatus> actual;
    for (const auto& [status, message] : parsed.errors)
    {
      actual.insert(status);
      std::cout << message << " " << configParseStatusToString(status) << std::endl;
    }
    EXPECT_EQ(actual, expected);
  }
}

TEST(ParseHenrysLaw, DetectsGasSpeciesInReactionNotFoundInGasPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        std::string("development_unit_configs/reactions/henrys_law/species_not_found_in_gas_phase") + extension;
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

TEST(ParseHenrysLaw, DetectsWhenRequestedSpeciesAreNotInAqueousPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = std::string("development_unit_configs/reactions/henrys_law/species_not_in_aqueous_phase") + extension;
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

TEST(ParseHenrysLaw, DetectsWhenRequestedSolventIsNotRegisteredInCorrectPhase)
{
  development::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file =
        std::string("development_unit_configs/reactions/henrys_law/solvent_species_not_registered_in_phase") + extension;
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

TEST(ValidateHenrysLaw, ValidationWithUnknownSpeciesUnknownPhaseFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "A" }, types::Species{ .name = "H2O" } };
  std::vector<types::Phase> existing_phases = {
    types::Phase{ .name = "gas", .species = { types::PhaseSpecies{ .name = "A" } } },
    types::Phase{ .name = "aqueous", .species = { types::PhaseSpecies{ .name = "H2O" } } }
  };

  YAML::Node reaction_node;
  reaction_node["type"] = "HL_PHASE_TRANSFER";

  reaction_node["gas"]["name"] = "gas";
  reaction_node["gas"]["species"] = YAML::Load("[{ name: A }]");

  // Particle with unknown phase and unknown species
  reaction_node["particle"]["phase"] = "unknown_phase";
  reaction_node["particle"]["solutes"] = YAML::Load("[{ name: H2O2, coefficient: 1.0 }]");
  reaction_node["particle"]["solvent"] = YAML::Load("[{ name: water, coefficient: 1.0 }]");

  HenrysLawParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 3);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::ReactionRequiresUnknownSpecies,
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

TEST(ValidateHenrysLaw, ValidationWithSpeciesNotInPhasesFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "A" },
                                                   types::Species{ .name = "B" },
                                                   types::Species{ .name = "C" },
                                                   types::Species{ .name = "H2O" } };
  std::vector<types::Phase> existing_phases = {
    types::Phase{ .name = "gas", .species = { types::PhaseSpecies{ .name = "A" } } },
    types::Phase{ .name = "aqueous", .species = { types::PhaseSpecies{ .name = "H2O" } } }
  };

  YAML::Node reaction_node;
  reaction_node["type"] = "HL_PHASE_TRANSFER";

  // Gas phase - species B exists but not in gas phase
  reaction_node["gas"]["name"] = "gas";
  reaction_node["gas"]["species"] = YAML::Load("[{ name: B }]");

  // Particle phase - species C exists but not in aqueous phase
  reaction_node["particle"]["phase"] = "aqueous";
  reaction_node["particle"]["solutes"] = YAML::Load("[{ name: C, coefficient: 1.0 }]");
  reaction_node["particle"]["solvent"] = YAML::Load("[{ name: H2O, coefficient: 1.0 }]");

  HenrysLawParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 2);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase,
                                                ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

TEST(ValidateHenrysLaw, ValidationWithMissingRequiredKeysFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "A" }, types::Species{ .name = "H2O" } };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas" }, types::Phase{ .name = "aqueous" } };

  YAML::Node reaction_node;
  reaction_node["type"] = "HL_PHASE_TRANSFER";
  // Missing required "gas" and "particle" keys

  HenrysLawParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 2);

  std::multiset<ConfigParseStatus> expected = { ConfigParseStatus::RequiredKeyNotFound,
                                                ConfigParseStatus::RequiredKeyNotFound };
  std::multiset<ConfigParseStatus> actual;
  for (const auto& [status, message] : errors)
  {
    actual.insert(status);
    std::cout << message << " " << configParseStatusToString(status) << std::endl;
  }
  EXPECT_EQ(actual, expected);
}

TEST(ValidateHenrysLaw, ValidationWithValidConfigurationPassesValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "A" },
                                                   types::Species{ .name = "B" },
                                                   types::Species{ .name = "H2O" } };
  std::vector<types::Phase> existing_phases = {
    types::Phase{ .name = "gas", .species = { types::PhaseSpecies{ .name = "A" } } },
    types::Phase{ .name = "aqueous",
                  .species = { types::PhaseSpecies{ .name = "B" }, types::PhaseSpecies{ .name = "H2O" } } }
  };

  YAML::Node reaction_node;
  reaction_node["type"] = "HL_PHASE_TRANSFER";

  reaction_node["gas"]["name"] = "gas";
  reaction_node["gas"]["species"] = YAML::Load("[{ name: A, \"diffusion coefficient [m2 s-1]\": 0.7 }]");

  reaction_node["particle"]["phase"] = "aqueous";
  reaction_node["particle"]["solutes"] = YAML::Load("[{ name: B, coefficient: 1.0 }]");
  reaction_node["particle"]["solvent"] = YAML::Load("[{ name: H2O, coefficient: 1.0 }]");

  // Optional name field
  reaction_node["name"] = "test henry's law reaction";

  HenrysLawParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 0);
}

TEST(ValidateHenrysLaw, ValidationWithMultipleSolutes)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "A" },
                                                   types::Species{ .name = "B" },
                                                   types::Species{ .name = "C" },
                                                   types::Species{ .name = "H2O" } };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas",
                                                              .species = { types::PhaseSpecies{ .name = "A" } } },
                                                types::Phase{ .name = "aqueous",
                                                              .species = { types::PhaseSpecies{ .name = "B" },
                                                                           types::PhaseSpecies{ .name = "C" },
                                                                           types::PhaseSpecies{ .name = "H2O" } } } };

  YAML::Node reaction_node;
  reaction_node["type"] = "HL_PHASE_TRANSFER";

  reaction_node["gas"]["name"] = "gas";
  reaction_node["gas"]["species"] = YAML::Load("[{ name: A }]");

  // Valid particle phase configuration with multiple solutes
  reaction_node["particle"]["phase"] = "aqueous";
  reaction_node["particle"]["solutes"] = YAML::Load("[{ name: B, coefficient: 1.0 }, { name: C, coefficient: 2.0 }]");
  reaction_node["particle"]["solvent"] = YAML::Load("[{ name: H2O, coefficient: 1.0 }]");

  HenrysLawParser parser;
  Errors errors = parser.Validate(reaction_node, existing_species, existing_phases);
  EXPECT_EQ(errors.size(), 0);
}

TEST(ValidateHenrysLaw, InvalidNumberSolventFailsValidation)
{
  using namespace development;

  std::vector<types::Species> existing_species = { types::Species{ .name = "A" },
                                                   types::Species{ .name = "B" },
                                                   types::Species{ .name = "C" },
                                                   types::Species{ .name = "H2O" },
                                                   types::Species{ .name = "water" } };
  std::vector<types::Phase> existing_phases = { types::Phase{ .name = "gas",
                                                              .species = { types::PhaseSpecies{ .name = "A" } } },
                                                types::Phase{ .name = "aqueous",
                                                              .species = { types::PhaseSpecies{ .name = "B" },
                                                                           types::PhaseSpecies{ .name = "C" },
                                                                           types::PhaseSpecies{ .name = "H2O" },
                                                                           types::PhaseSpecies{ .name = "water" } } } };

  YAML::Node reaction_node;
  reaction_node["type"] = "HL_PHASE_TRANSFER";

  reaction_node["gas"]["name"] = "gas";
  reaction_node["gas"]["species"] = YAML::Load("[{ name: A }]");
  reaction_node["particle"]["phase"] = "aqueous";
  reaction_node["particle"]["solutes"] = YAML::Load("[{ name: B, coefficient: 1.0 }, { name: C, coefficient: 2.0 }]");

  // The invalid number of the solvent triggers validation error
  reaction_node["particle"]["solvent"] = YAML::Load("[{ name: H2O, coefficient: 1.0 }, { name: water, coefficient: 1.0 }]");

  HenrysLawParser parser;
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
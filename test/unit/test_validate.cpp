// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/validate.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

namespace
{
  types::Species species(const std::string& name)
  {
    types::Species s;
    s.name = name;
    return s;
  }
  types::PhaseSpecies phase_species(const std::string& name)
  {
    types::PhaseSpecies ps;
    ps.name = name;
    return ps;
  }
  types::ReactionComponent component(const std::string& name)
  {
    types::ReactionComponent c;
    c.name = name;
    return c;
  }

  types::Inventory inventory(const std::string& name)
  {
    types::Inventory inv;
    inv.name = name;
    inv.directory = "data";
    inv.file_pattern = "file_{YYYY}.nc";
    inv.convention = "uptempo";
    return inv;
  }

  types::SpeciesMapping
  mapping(const std::string& inventory_species, const std::string& mechanism_species, double scaling_factor = 1.0)
  {
    types::SpeciesMapping m;
    m.inventory_species = inventory_species;
    m.mechanism_species = mechanism_species;
    m.scaling_factor = scaling_factor;
    return m;
  }

  types::SpeciesMap species_map(const std::string& name, std::vector<types::SpeciesMapping> mappings)
  {
    types::SpeciesMap smap;
    smap.name = name;
    smap.mappings = std::move(mappings);
    return smap;
  }

  types::SourceDescriptor source(
      const std::string& name,
      const std::string& inventory_name,
      const std::string& species_map_name,
      int category = 0,
      int hierarchy = 1)
  {
    types::SourceDescriptor src;
    src.name = name;
    src.inventory = inventory_name;
    src.species_map = species_map_name;
    src.category = category;
    src.hierarchy = hierarchy;
    return src;
  }

  // gas phase {A, B}, aqueous phase {C}; species A, B, C.
  Mechanism BaseMechanism()
  {
    Mechanism m;
    m.species = { species("A"), species("B"), species("C") };
    types::Phase gas;
    gas.name = "gas";
    gas.species = { phase_species("A"), phase_species("B") };
    types::Phase aqueous;
    aqueous.name = "aqueous";
    aqueous.species = { phase_species("C") };
    m.phases = { gas, aqueous };
    return m;
  }

  bool HasCode(const Errors& errors, ErrorCode code)
  {
    for (const auto& [c, m] : errors)
      if (c == code)
        return true;
    return false;
  }
}  // namespace

// An in-code mechanism (never parsed from a file) can be validated.
TEST(Validate, AcceptsValidInCodeMechanism)
{
  Mechanism m = BaseMechanism();
  types::Arrhenius rxn;
  rxn.gas_phase = "gas";
  rxn.reactants = { component("A") };
  rxn.products = { component("B") };
  m.reactions.arrhenius = { rxn };

  EXPECT_TRUE(Validate(m).empty());
}

TEST(Validate, DetectsUnknownSpeciesInReaction)
{
  Mechanism m = BaseMechanism();
  types::Arrhenius rxn;
  rxn.gas_phase = "gas";
  rxn.reactants = { component("Z") };  // not in the species list
  m.reactions.arrhenius = { rxn };

  auto errors = Validate(m);
  EXPECT_TRUE(HasCode(errors, ErrorCode::ReactionRequiresUnknownSpecies));
}

TEST(Validate, DetectsReactantNotInReactionPhase)
{
  Mechanism m = BaseMechanism();
  types::Arrhenius rxn;
  rxn.gas_phase = "gas";
  rxn.reactants = { component("C") };  // C is known, but only in the aqueous phase
  m.reactions.arrhenius = { rxn };

  auto errors = Validate(m);
  EXPECT_TRUE(HasCode(errors, ErrorCode::RequestedSpeciesNotRegisteredInPhase));
}

// Products may reference species from any phase.
TEST(Validate, AllowsCrossPhaseProduct)
{
  Mechanism m = BaseMechanism();
  types::Arrhenius rxn;
  rxn.gas_phase = "gas";
  rxn.reactants = { component("A") };
  rxn.products = { component("C") };  // C lives in aqueous; allowed as a product
  m.reactions.arrhenius = { rxn };

  EXPECT_TRUE(Validate(m).empty());
}

TEST(Validate, DetectsDuplicateSpecies)
{
  Mechanism m = BaseMechanism();
  m.species.push_back(species("A"));  // duplicate

  EXPECT_TRUE(HasCode(Validate(m), ErrorCode::DuplicateSpeciesDetected));
}

TEST(Validate, DetectsUnknownPhase)
{
  Mechanism m = BaseMechanism();
  types::Arrhenius rxn;
  rxn.gas_phase = "stratosphere";  // no such phase
  rxn.reactants = { component("A") };
  m.reactions.arrhenius = { rxn };

  EXPECT_TRUE(HasCode(Validate(m), ErrorCode::UnknownPhase));
}

// An in-code mechanism with a well-formed emissions section validates cleanly.
TEST(Validate, EmissionsValidConfigAccepted)
{
  Mechanism m = BaseMechanism();
  types::EmissionsConfig em;
  em.inventories = { inventory("cams bc") };
  em.species_maps = { species_map("bc map", { mapping("bc_anth_sum", "BC") }) };
  em.sources = { source("CAMS black carbon", "cams bc", "bc map") };
  m.emissions = em;

  EXPECT_TRUE(Validate(m).empty());
}

// A mechanism with no emissions section at all is unaffected (regression guard for the
// std::optional<EmissionsConfig> branch).
TEST(Validate, AcceptsMechanismWithoutEmissions)
{
  Mechanism m = BaseMechanism();
  EXPECT_TRUE(Validate(m).empty());
}

TEST(Validate, EmissionsDetectsDuplicateInventory)
{
  Mechanism m = BaseMechanism();
  types::EmissionsConfig em;
  em.inventories = { inventory("cams bc"), inventory("cams bc") };
  m.emissions = em;

  EXPECT_TRUE(HasCode(Validate(m), ErrorCode::DuplicateInventoryDetected));
}

TEST(Validate, EmissionsDetectsDuplicateSpeciesMap)
{
  Mechanism m = BaseMechanism();
  types::EmissionsConfig em;
  em.species_maps = { species_map("bc map", {}), species_map("bc map", {}) };
  m.emissions = em;

  EXPECT_TRUE(HasCode(Validate(m), ErrorCode::DuplicateSpeciesMapDetected));
}

TEST(Validate, EmissionsDetectsDuplicateSource)
{
  Mechanism m = BaseMechanism();
  types::EmissionsConfig em;
  em.inventories = { inventory("cams bc") };
  em.species_maps = { species_map("bc map", {}) };
  em.sources = { source("CAMS black carbon", "cams bc", "bc map", 0, 1),
                 source("CAMS black carbon", "cams bc", "bc map", 1, 1) };
  m.emissions = em;

  EXPECT_TRUE(HasCode(Validate(m), ErrorCode::DuplicateSourceDetected));
}

TEST(Validate, EmissionsDetectsDuplicateCategoryHierarchy)
{
  Mechanism m = BaseMechanism();
  types::EmissionsConfig em;
  em.inventories = { inventory("cams bc") };
  em.species_maps = { species_map("bc map", {}) };
  em.sources = { source("source one", "cams bc", "bc map", 0, 1), source("source two", "cams bc", "bc map", 0, 1) };
  m.emissions = em;

  EXPECT_TRUE(HasCode(Validate(m), ErrorCode::DuplicateCategoryHierarchy));
}

// The check runs unconditionally, even when `inventories` is empty/absent — a source cannot
// reference an inventory that was never declared.
TEST(Validate, EmissionsDetectsSourceWithUnknownInventory)
{
  Mechanism m = BaseMechanism();
  types::EmissionsConfig em;
  em.species_maps = { species_map("bc map", {}) };
  em.sources = { source("CAMS black carbon", "nonexistent inventory", "bc map") };
  m.emissions = em;

  EXPECT_TRUE(HasCode(Validate(m), ErrorCode::SourceRequiresUnknownInventory));
}

TEST(Validate, EmissionsDetectsSourceWithUnknownSpeciesMap)
{
  Mechanism m = BaseMechanism();
  types::EmissionsConfig em;
  em.inventories = { inventory("cams bc") };
  em.sources = { source("CAMS black carbon", "cams bc", "nonexistent map") };
  m.emissions = em;

  EXPECT_TRUE(HasCode(Validate(m), ErrorCode::SourceRequiresUnknownSpeciesMap));
}

TEST(Validate, EmissionsDetectsScalingFactorExceedsOne)
{
  Mechanism m = BaseMechanism();
  types::EmissionsConfig em;
  em.species_maps = { species_map("bad map", { mapping("NOx", "NO", 0.9), mapping("NOx", "NO2", 0.5) }) };
  m.emissions = em;

  EXPECT_TRUE(HasCode(Validate(m), ErrorCode::SpeciesMapScalingExceedsOne));
}

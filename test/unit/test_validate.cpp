// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/validate.hpp>

#include <gtest/gtest.h>

#include <optional>

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

namespace
{
  // species A (mw), H2O (mw); gas {A: diffusion}, aqueous {A, H2O: density};
  // one UNIFORM_SECTION representation "cloud" over the aqueous phase.
  Mechanism AerosolBaseMechanism()
  {
    Mechanism m;
    types::Species a = species("A");
    a.molecular_weight = 0.05;
    types::Species h2o = species("H2O");
    h2o.molecular_weight = 0.018;
    m.species = { a, h2o };

    types::Phase gas;
    gas.name = "gas";
    types::PhaseSpecies gas_a = phase_species("A");
    gas_a.diffusion_coefficient = 1.5e-5;
    gas.species = { gas_a };

    types::Phase aqueous;
    aqueous.name = "aqueous";
    types::PhaseSpecies aqueous_h2o = phase_species("H2O");
    aqueous_h2o.density = 1000.0;
    aqueous.species = { phase_species("A"), aqueous_h2o };

    m.phases = { gas, aqueous };

    types::UniformSection section;
    section.name = "cloud";
    section.phases = { "aqueous" };
    section.min_radius = 1.0e-6;
    section.max_radius = 1.0e-5;
    m.aerosol.representations = { section };

    return m;
  }

  types::HenryLawPhaseTransfer ValidPhaseTransfer()
  {
    types::HenryLawPhaseTransfer t;
    t.gas_phase = "gas";
    t.gas_species = "A";
    t.condensed_phase = "aqueous";
    t.condensed_species = "A";
    t.solvent = "H2O";
    return t;
  }

  types::HenryLawEquilibrium ValidEquilibrium()
  {
    types::HenryLawEquilibrium e;
    e.gas_phase = "gas";
    e.gas_species = "A";
    e.condensed_phase = "aqueous";
    e.condensed_species = "A";
    e.solvent = "H2O";
    return e;
  }
}  // namespace

TEST(ValidateAerosol, IgnoresMechanismWithoutAerosolSection)
{
  EXPECT_TRUE(ValidateAerosolModel(BaseMechanism()).empty());
}

TEST(ValidateAerosol, AcceptsValidProcessesAndConstraints)
{
  Mechanism m = AerosolBaseMechanism();
  m.aerosol.processes = { ValidPhaseTransfer() };

  types::DissolvedReaction reaction;
  reaction.phase = "aqueous";
  reaction.solvent = "H2O";
  reaction.reactants = { component("A") };
  reaction.products = { component("A") };
  reaction.rate_constants = { { "cloud", types::ArrheniusReferenceTemperature{} } };  // keyed by a declared representation
  m.aerosol.processes.push_back(reaction);

  m.aerosol.constraints = { ValidEquilibrium() };

  EXPECT_TRUE(ValidateAerosolModel(m).empty());
}

TEST(ValidateAerosol, DetectsRepresentationReferencingUnknownPhase)
{
  Mechanism m = AerosolBaseMechanism();
  types::SingleMomentMode mode;
  mode.name = "mode";
  mode.phases = { "nonexistent" };  // no such phase
  mode.geometric_mean_radius = 1.0e-6;
  mode.geometric_standard_deviation = 1.6;
  m.aerosol.representations.push_back(mode);

  EXPECT_TRUE(HasCode(ValidateAerosolModel(m), ErrorCode::UnknownPhase));
}

TEST(ValidateAerosol, DetectsSpeciesNotRegisteredInCondensedPhase)
{
  Mechanism m = AerosolBaseMechanism();
  types::DissolvedReaction reaction;
  reaction.phase = "aqueous";
  reaction.solvent = "H2O";
  reaction.reactants = { component("Q") };  // Q is not registered in the aqueous phase
  m.aerosol.processes = { reaction };

  EXPECT_TRUE(HasCode(ValidateAerosolModel(m), ErrorCode::RequestedSpeciesNotRegisteredInPhase));
}

TEST(ValidateAerosol, DetectsRateConstantKeyedByUnknownRepresentation)
{
  Mechanism m = AerosolBaseMechanism();
  types::DissolvedReaction reaction;
  reaction.phase = "aqueous";
  reaction.solvent = "H2O";
  reaction.reactants = { component("A") };
  reaction.products = { component("A") };
  reaction.rate_constants = { { "not_a_representation", types::ArrheniusReferenceTemperature{} } };
  m.aerosol.processes = { reaction };

  EXPECT_TRUE(HasCode(ValidateAerosolModel(m), ErrorCode::UnknownAerosolRepresentation));
}

TEST(ValidateAerosol, DetectsMissingGasDiffusionCoefficient)
{
  Mechanism m = AerosolBaseMechanism();
  m.phases[0].species[0].diffusion_coefficient = std::nullopt;  // gas-phase A loses its diffusion coefficient
  m.aerosol.processes = { ValidPhaseTransfer() };

  EXPECT_TRUE(HasCode(ValidateAerosolModel(m), ErrorCode::RequiredKeyNotFound));
}

TEST(ValidateAerosol, DetectsMissingSolventDensity)
{
  Mechanism m = AerosolBaseMechanism();
  m.phases[1].species[1].density = std::nullopt;  // aqueous H2O loses its density
  m.aerosol.constraints = { ValidEquilibrium() };

  EXPECT_TRUE(HasCode(ValidateAerosolModel(m), ErrorCode::RequiredKeyNotFound));
}

TEST(ValidateAerosol, DetectsMissingSolventMolecularWeight)
{
  Mechanism m = AerosolBaseMechanism();
  m.species[1].molecular_weight = std::nullopt;  // H2O loses its molecular weight
  m.aerosol.constraints = { ValidEquilibrium() };

  EXPECT_TRUE(HasCode(ValidateAerosolModel(m), ErrorCode::RequiredKeyNotFound));
}

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

  EXPECT_TRUE(validate(m).empty());
}

TEST(Validate, DetectsUnknownSpeciesInReaction)
{
  Mechanism m = BaseMechanism();
  types::Arrhenius rxn;
  rxn.gas_phase = "gas";
  rxn.reactants = { component("Z") };  // not in the species list
  m.reactions.arrhenius = { rxn };

  auto errors = validate(m);
  EXPECT_TRUE(HasCode(errors, ErrorCode::ReactionRequiresUnknownSpecies));
}

TEST(Validate, DetectsReactantNotInReactionPhase)
{
  Mechanism m = BaseMechanism();
  types::Arrhenius rxn;
  rxn.gas_phase = "gas";
  rxn.reactants = { component("C") };  // C is known, but only in the aqueous phase
  m.reactions.arrhenius = { rxn };

  auto errors = validate(m);
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

  EXPECT_TRUE(validate(m).empty());
}

TEST(Validate, DetectsDuplicateSpecies)
{
  Mechanism m = BaseMechanism();
  m.species.push_back(species("A"));  // duplicate

  EXPECT_TRUE(HasCode(validate(m), ErrorCode::DuplicateSpeciesDetected));
}

TEST(Validate, DetectsUnknownPhase)
{
  Mechanism m = BaseMechanism();
  types::Arrhenius rxn;
  rxn.gas_phase = "stratosphere";  // no such phase
  rxn.reactants = { component("A") };
  m.reactions.arrhenius = { rxn };

  EXPECT_TRUE(HasCode(validate(m), ErrorCode::UnknownPhase));
}

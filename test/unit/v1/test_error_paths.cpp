// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/parse.hpp>
#include <mechanism_configuration/validate.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <optional>
#include <string>
#include <vector>

using namespace mechanism_configuration;

namespace
{
  // A valid v1 document with the given species, phases, and reactions (JSON arrays).
  std::string Document(const std::string& species, const std::string& phases, const std::string& reactions)
  {
    return R"({ "version": "1.0.0", "species": )" + species + R"(, "phases": )" + phases + R"(, "reactions": )" + reactions +
           "}";
  }

  const std::string kSpecies = R"([ { "name": "A" }, { "name": "B" } ])";
  const std::string kPhases = R"([ { "name": "gas", "species": [ "A", "B" ] } ])";

  // A reaction A -> B in the gas phase, with `extra` added to its keys.
  std::string Reaction(const std::string& extra = "")
  {
    return R"({ "type": "ARRHENIUS", "gas phase": "gas", "reactants": [ { "name": "A" } ],
                "products": [ { "name": "B" } ])" +
           extra + "}";
  }

  using Paths = std::vector<std::pair<ErrorCode, std::optional<std::string>>>;

  // Prints the errors and gives their (code, path) pairs.
  Paths PrintAndCollect(const Errors& errors)
  {
    Paths result;
    for (const auto& error : errors)
    {
      std::cout << ErrorCodeToString(error.code) << " at " << (error.path ? "\"" + *error.path + "\"" : "<no path>") << ": "
                << error.message << std::endl;
      result.emplace_back(error.code, error.path);
    }
    return result;
  }

  // The (code, path) pairs of the errors from parsing `document`.
  Paths ErrorPaths(const std::string& document)
  {
    auto parsed = ParseFromString(document);
    EXPECT_FALSE(parsed);
    return parsed ? Paths{} : PrintAndCollect(parsed.error());
  }
}  // namespace

TEST(ErrorPaths, MissingKeyPointsToTheObject)
{
  const auto document = Document(R"([ { "name": "A" }, { "molecular weight [kg mol-1]": 0.1 } ])", kPhases, "[]");
  EXPECT_EQ(ErrorPaths(document), (Paths{ { ErrorCode::RequiredKeyNotFound, "/species/1" } }));
}

// Schema errors point to the item of a top-level list, not to the key in the item.
TEST(ErrorPaths, NonStandardKeyPointsToTheItem)
{
  const auto document = Document(kSpecies, kPhases, "[" + Reaction(R"(, "a/b~c": 1)") + "]");
  EXPECT_EQ(ErrorPaths(document), (Paths{ { ErrorCode::InvalidKey, "/reactions/0" } }));
}

TEST(ErrorPaths, SchemaErrorInAReactionComponentPointsToTheReaction)
{
  const auto reaction = R"({ "type": "ARRHENIUS", "gas phase": "gas", "reactants": [ { "name": "A" } ],
                             "products": [ { "name": "B", "species name": "B" } ] })";
  const auto document = Document(kSpecies, kPhases, std::string("[") + reaction + "]");
  EXPECT_EQ(ErrorPaths(document), (Paths{ { ErrorCode::MutuallyExclusiveOption, "/reactions/0" } }));
}

TEST(ErrorPaths, UnknownReactionTypePointsToTheReaction)
{
  const auto document = Document(kSpecies, kPhases, "[" + Reaction() + R"(, { "type": "NOT_A_TYPE" } ])");
  EXPECT_EQ(ErrorPaths(document), (Paths{ { ErrorCode::UnknownType, "/reactions/1" } }));
}

// Semantic errors point to the reference in the item.
TEST(ErrorPaths, SemanticErrorsPointToTheReferences)
{
  // B is not in the gas phase, so the product of the second reaction is not allowed.
  const auto phases = R"([ { "name": "gas", "species": [ "A" ] }, { "name": "aqueous", "species": [ "B" ] } ])";
  const auto unknown_phase = R"({ "type": "ARRHENIUS", "gas phase": "nowhere", "reactants": [ { "name": "A" } ],
                                  "products": [] })";
  const auto document = Document(kSpecies, phases, std::string("[") + unknown_phase + ", " + Reaction() + "]");
  EXPECT_EQ(
      ErrorPaths(document),
      (Paths{ { ErrorCode::UnknownPhase, "/reactions/0/gas phase" },
              { ErrorCode::RequestedSpeciesNotRegisteredInPhase, "/reactions/1/products/0" } }));
}

TEST(ErrorPaths, DuplicatesPointToEachOccurrence)
{
  const auto species = R"([ { "name": "A" }, { "name": "B" }, { "name": "A" } ])";
  const auto phases = R"([ { "name": "gas", "species": [ "A", "C" ] } ])";
  const auto document = Document(species, phases, "[]");
  EXPECT_EQ(
      ErrorPaths(document),
      (Paths{ { ErrorCode::DuplicateSpeciesDetected, "/species/0" },
              { ErrorCode::DuplicateSpeciesDetected, "/species/2" },
              { ErrorCode::PhaseRequiresUnknownSpecies, "/phases/0/species/1" } }));
}

TEST(ErrorPaths, MissingVersionPointsToTheDocument)
{
  EXPECT_EQ(ErrorPaths(R"({ "species": [], "phases": [] })"), (Paths{ { ErrorCode::InvalidVersion, "" } }));
}

// A Mechanism in code has no document, so its paths point into the struct.
TEST(ErrorPaths, InCodeMechanismPointsIntoTheStruct)
{
  Mechanism mechanism;
  mechanism.species = { { .name = "A" }, { .name = "B" } };
  mechanism.phases = { { .name = "gas", .species = { { .name = "A" } } } };
  types::Arrhenius reaction;
  reaction.gas_phase = "gas";
  reaction.reactants = { { .name = "A" } };
  reaction.products = { { .name = "B" } };
  types::Troe unknown_species;
  unknown_species.gas_phase = "gas";
  unknown_species.reactants = { { .name = "C" } };
  mechanism.reactions.arrhenius = { reaction };
  mechanism.reactions.troe = { unknown_species };

  EXPECT_EQ(
      PrintAndCollect(Validate(mechanism)),
      (Paths{ { ErrorCode::RequestedSpeciesNotRegisteredInPhase, "/reactions/arrhenius/0/products/0" },
              { ErrorCode::ReactionRequiresUnknownSpecies, "/reactions/troe/0/reactants/0" } }));
}

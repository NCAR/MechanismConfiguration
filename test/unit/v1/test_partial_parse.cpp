// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <optional>
#include <string>
#include <vector>

using namespace mechanism_configuration;

namespace
{
  // A v1 document with the given species, phases, and reactions (JSON arrays).
  std::string Document(const std::string& species, const std::string& phases, const std::string& reactions)
  {
    return R"({ "version": "1.0.0", "species": )" + species + R"(, "phases": )" + phases + R"(, "reactions": )" + reactions +
           "}";
  }

  const std::string kSpecies = R"([ { "name": "A" }, { "name": "B" } ])";
  const std::string kPhases = R"([ { "name": "gas", "species": [ "A", "B" ] } ])";
  const std::string kReaction =
      R"({ "type": "ARRHENIUS", "gas phase": "gas", "reactants": [ { "name": "A" } ], "products": [ { "name": "B" } ] })";

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
}  // namespace

TEST(PartialParse, ValidDocumentGivesTheFullMechanismWithoutErrors)
{
  const auto document = Document(kSpecies, kPhases, "[" + kReaction + "]");
  auto parsed = ParseFromString(document);
  ASSERT_TRUE(parsed);
  EXPECT_TRUE(parsed.errors.empty());
  EXPECT_EQ(parsed.mechanism->species.size(), 2);
  EXPECT_EQ(parsed.mechanism->reactions.arrhenius.size(), 1);
}

// The parser leaves out only the reaction that has no parser, and tells that it is left out.
TEST(PartialParse, LeavesOutAReactionThatCannotBeBuilt)
{
  const auto document = Document(kSpecies, kPhases, "[" + kReaction + R"(, { "type": "NOT_A_TYPE" }, )" + kReaction + "]");

  auto parsed = ParseFromString(document);
  ASSERT_TRUE(parsed.mechanism);
  EXPECT_EQ(
      PrintAndCollect(parsed.errors),
      (Paths{ { ErrorCode::UnknownType, "/reactions/1" }, { ErrorCode::ItemLeftOut, "/reactions/1" } }));
  EXPECT_EQ(parsed.mechanism->reactions.arrhenius.size(), 2);
}

// An item with a schema error that does not stop the build is kept, with the error.
TEST(PartialParse, KeepsAnItemWithASchemaErrorThatCanBeBuilt)
{
  const auto extra_key =
      R"({ "type": "ARRHENIUS", "gas phase": "gas", "typo": 1, "reactants": [ { "name": "A" } ], "products": [] })";
  const auto both_names =
      R"({ "type": "ARRHENIUS", "gas phase": "gas", "reactants": [ { "name": "A", "species name": "A" } ], "products": [] })";
  const auto document = Document(kSpecies, kPhases, std::string("[") + extra_key + ", " + both_names + "]");

  auto parsed = ParseFromString(document);
  ASSERT_TRUE(parsed.mechanism);
  EXPECT_EQ(
      PrintAndCollect(parsed.errors),
      (Paths{ { ErrorCode::InvalidKey, "/reactions/0" }, { ErrorCode::MutuallyExclusiveOption, "/reactions/1" } }));
  ASSERT_EQ(parsed.mechanism->reactions.arrhenius.size(), 2);
  EXPECT_EQ(parsed.mechanism->reactions.arrhenius[1].reactants[0].name, "A");
}

// A species without a name is left out. A reaction that uses a species that is not built gets a
// semantic error, at its path in the document.
TEST(PartialParse, ChecksTheReferencesOfTheBuiltItems)
{
  const auto species = R"([ { "molecular weight [kg mol-1]": 0.1 }, { "name": "A" } ])";
  const auto phases = R"([ { "name": "gas", "species": [ "A" ] } ])";
  const auto uses_b =
      R"({ "type": "ARRHENIUS", "gas phase": "gas", "reactants": [ { "name": "A" } ], "products": [ { "name": "B" } ] })";
  const auto document = Document(species, phases, std::string(R"([ { "type": "NOT_A_TYPE" }, )") + uses_b + "]");

  auto parsed = ParseFromString(document);
  ASSERT_TRUE(parsed.mechanism);
  EXPECT_EQ(
      PrintAndCollect(parsed.errors),
      (Paths{ { ErrorCode::RequiredKeyNotFound, "/species/0" },
              { ErrorCode::UnknownType, "/reactions/0" },
              { ErrorCode::ItemLeftOut, "/species/0" },
              { ErrorCode::ItemLeftOut, "/reactions/0" },
              { ErrorCode::ReactionRequiresUnknownSpecies, "/reactions/1/products/0" } }));
  ASSERT_EQ(parsed.mechanism->species.size(), 1);
  EXPECT_EQ(parsed.mechanism->species[0].name, "A");
  EXPECT_EQ(parsed.mechanism->reactions.arrhenius.size(), 1);
}

TEST(PartialParse, BuildsTheMechanismWhenOnlySemanticErrorsExist)
{
  const auto phases = R"([ { "name": "gas", "species": [ "A" ] } ])";
  const auto document = Document(kSpecies, phases, "[" + kReaction + "]");

  auto parsed = ParseFromString(document);
  ASSERT_TRUE(parsed.mechanism);
  EXPECT_EQ(
      PrintAndCollect(parsed.errors),
      (Paths{ { ErrorCode::RequestedSpeciesNotRegisteredInPhase, "/reactions/0/products/0" } }));
  EXPECT_EQ(parsed.mechanism->phases.size(), 1);
  EXPECT_EQ(parsed.mechanism->reactions.arrhenius.size(), 1);
}

// A missing section is reported and is empty. The other sections are still checked and built.
TEST(PartialParse, BuildsTheOtherSectionsWhenASectionIsMissing)
{
  const auto document = R"({ "version": "1.0.0", "species": [ { "name": "A" } ], "reactions": [ )" + kReaction + " ] }";

  auto parsed = ParseFromString(document);
  ASSERT_TRUE(parsed.mechanism);
  const auto paths = PrintAndCollect(parsed.errors);
  ASSERT_FALSE(paths.empty());
  EXPECT_EQ(paths[0], (std::pair<ErrorCode, std::optional<std::string>>{ ErrorCode::RequiredKeyNotFound, "" }));
  // The message has the location once, with the empty file path in front of it.
  EXPECT_EQ(parsed.errors[0].message.substr(0, 5), ":1:1 ");
  EXPECT_EQ(parsed.mechanism->species.size(), 1);
  EXPECT_TRUE(parsed.mechanism->phases.empty());
  EXPECT_EQ(parsed.mechanism->reactions.arrhenius.size(), 1);
}

// The aerosol sections refer to each other, so a problem leaves out both of them.
TEST(PartialParse, LeavesOutTheAerosolSectionsWhenOneIsMissing)
{
  const auto document = R"({ "version": "1.2.0", "species": )" + kSpecies + R"(, "phases": )" + kPhases +
                        R"(, "reactions": [ )" + kReaction + R"( ], "aerosol representations": [] })";

  auto parsed = ParseFromString(document);
  ASSERT_TRUE(parsed.mechanism);
  EXPECT_EQ(PrintAndCollect(parsed.errors), (Paths{ { ErrorCode::RequiredKeyNotFound, "" } }));
  EXPECT_FALSE(parsed.mechanism->aerosol.has_value());
  EXPECT_EQ(parsed.mechanism->reactions.arrhenius.size(), 1);
}

// A result with errors converts to false, also when it has a mechanism.
TEST(PartialParse, ConvertsToFalseWhenErrorsExist)
{
  const auto phases = R"([ { "name": "gas", "species": [ "A" ] } ])";
  auto parsed = ParseFromString(Document(kSpecies, phases, "[" + kReaction + "]"));
  EXPECT_TRUE(parsed.mechanism);
  EXPECT_FALSE(parsed.errors.empty());
  EXPECT_FALSE(parsed);
}

TEST(PartialParse, GivesNoMechanismWhenTheDocumentCannotBeRead)
{
  for (const auto& document : { std::string("{ not: [ valid"),
                                std::string(R"({ "species": [], "phases": [] })"),
                                std::string(R"({ "version": "2.0.0", "species": [], "phases": [] })") })
  {
    auto parsed = ParseFromString(document);
    EXPECT_FALSE(parsed.mechanism) << document;
    EXPECT_FALSE(parsed.errors.empty()) << document;
  }
}

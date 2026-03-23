// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/parse_status.hpp>
#include <mechanism_configuration/v1/parser.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

const std::string configBase = "test/unit/v1/file_configs/configs/";

// Helper: collect all error status codes from a result
static std::vector<ConfigParseStatus> ErrorStatuses(const ParserResult<v1::types::Mechanism>& result)
{
  std::vector<ConfigParseStatus> statuses;
  for (const auto& error : result.errors)
    statuses.push_back(error.first);
  return statuses;
}

// ── two_species_sets ──────────────────────────────────────────────────────────
// species_set_1 (A,B,C) + species_set_2 (D,E,F) merged into 6 species
// single gas phase, 6 arrhenius reactions (3 tropo + 3 strat)

TEST(ParseFromFileConfigs, TwoSpeciesSets)
{
  v1::Parser parser;
  auto parsed = parser.Parse(configBase + "two_species_sets/main.json");
  for (const auto& error : parsed.errors)
    std::cout << "[" << configParseStatusToString(error.first) << "] " << error.second << "\n";
  ASSERT_TRUE(parsed);

  auto& mechanism = *parsed;
  EXPECT_EQ(mechanism.name, "troposphere and stratosphere configs");

  ASSERT_EQ(mechanism.species.size(), 6);
  EXPECT_EQ(mechanism.species[0].name, "A");
  EXPECT_EQ(mechanism.species[1].name, "B");
  EXPECT_EQ(mechanism.species[2].name, "C");
  EXPECT_EQ(mechanism.species[3].name, "D");
  EXPECT_EQ(mechanism.species[4].name, "E");
  EXPECT_EQ(mechanism.species[5].name, "F");

  ASSERT_EQ(mechanism.phases.size(), 1);
  EXPECT_EQ(mechanism.phases[0].name, "gas");
  ASSERT_EQ(mechanism.phases[0].species.size(), 3);

  EXPECT_EQ(mechanism.reactions.arrhenius.size(), 6);
}

// ── two_phases_sets ───────────────────────────────────────────────────────────
// gas_phase_1 (gas: A,B) + gas_phase_2 (aqueous: C) merged into 2 phases
// 2 photolysis (tropo) + 3 arrhenius (strat)

TEST(ParseFromFileConfigs, TwoPhasesSets)
{
  v1::Parser parser;
  auto parsed = parser.Parse(configBase + "two_phases_sets/main.json");
  for (const auto& error : parsed.errors)
    std::cout << "[" << configParseStatusToString(error.first) << "] " << error.second << "\n";
  ASSERT_TRUE(parsed);

  auto& mechanism = *parsed;

  ASSERT_EQ(mechanism.species.size(), 3);

  ASSERT_EQ(mechanism.phases.size(), 2);
  EXPECT_EQ(mechanism.phases[0].name, "gas");
  EXPECT_EQ(mechanism.phases[1].name, "aqueous");

  EXPECT_EQ(mechanism.reactions.photolysis.size(), 2);
  EXPECT_EQ(mechanism.reactions.arrhenius.size(), 3);
}

// ── missing_phase_set ─────────────────────────────────────────────────────────
// phases section has no "files" key : RequiredKeyNotFound

TEST(ParseFromFileConfigs, MissingPhaseSet)
{
  v1::Parser parser;
  auto parsed = parser.Parse(configBase + "missing_phase_set/main.json");

  EXPECT_FALSE(parsed);

  auto statuses = ErrorStatuses(parsed);
  EXPECT_TRUE(
      std::find(statuses.begin(), statuses.end(), ConfigParseStatus::RequiredKeyNotFound) !=
      statuses.end());
}

// ── missing_reaction_set ──────────────────────────────────────────────────────
// reactions.files is empty [] : parser succeeds with 0 reactions

TEST(ParseFromFileConfigs, MissingReactionSet)
{
  v1::Parser parser;
  auto parsed = parser.Parse(configBase + "missing_reaction_set/main.json");
  for (const auto& error : parsed.errors)
    std::cout << "[" << configParseStatusToString(error.first) << "] " << error.second << "\n";
  ASSERT_TRUE(parsed);

  auto& mechanism = *parsed;
  EXPECT_EQ(mechanism.reactions.arrhenius.size(), 0);
  EXPECT_EQ(mechanism.reactions.photolysis.size(), 0);
}

// ── missing_species_set ───────────────────────────────────────────────────────
// "species" key absent from main.json : RequiredKeyNotFound from ValidateSchema

TEST(ParseFromFileConfigs, MissingSpeciesSet)
{
  v1::Parser parser;
  auto parsed = parser.Parse(configBase + "missing_species_set/main.json");

  EXPECT_FALSE(parsed);

  auto statuses = ErrorStatuses(parsed);
  EXPECT_TRUE(
      std::find(statuses.begin(), statuses.end(), ConfigParseStatus::RequiredKeyNotFound) !=
      statuses.end());
}

// ── version_mismatch ──────────────────────────────────────────────────────────
// version "1.3.0" : minor != 1 for file-list format : InvalidVersion

TEST(ParseFromFileConfigs, VersionMismatch)
{
  v1::Parser parser;
  auto parsed = parser.Parse(configBase + "version_mismatch/main.json");

  EXPECT_FALSE(parsed);

  auto statuses = ErrorStatuses(parsed);
  EXPECT_TRUE(
      std::find(statuses.begin(), statuses.end(), ConfigParseStatus::InvalidVersion) !=
      statuses.end());
}

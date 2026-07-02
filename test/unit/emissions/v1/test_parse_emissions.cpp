// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/parser.hpp"

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

#include <filesystem>

using namespace mechanism_configuration;

static auto ParseFile(const char* p)
{
  return Parse(std::filesystem::path(p));
}

static auto ParseString(const std::string& content)
{
  return v1::Parser{}.Parse(content);
}

TEST(EmissionsV1Parser, ParsesValidConfig)
{
  auto result = ParseFile("emissions_unit_configs/valid.yaml");
  ASSERT_TRUE(result) << "Parse failed: " << (result ? "" : result.error()[0].second);

  ASSERT_TRUE(result->emissions.has_value());
  const auto& em = *result->emissions;

  ASSERT_EQ(em.inventories.size(), 1u);
  EXPECT_EQ(em.inventories[0].name, "cams bc");
  EXPECT_EQ(em.inventories[0].directory, "cams");
  EXPECT_EQ(em.inventories[0].file_pattern, "CAMS-GLOB-ANT_{YYYY}-{MM}.nc");
  EXPECT_EQ(em.inventories[0].convention, "uptempo");

  ASSERT_EQ(em.species_maps.size(), 1u);
  const auto& smap = em.species_maps[0];
  EXPECT_EQ(smap.name, "bc map");
  ASSERT_EQ(smap.mappings.size(), 1u);
  EXPECT_EQ(smap.mappings[0].inventory_species, "bc_anth_sum");
  EXPECT_EQ(smap.mappings[0].mechanism_species, "BC");
  EXPECT_DOUBLE_EQ(smap.mappings[0].scaling_factor, 1.0);

  EXPECT_EQ(em.regridding.type, types::RegriddingType::None);

  ASSERT_EQ(em.sources.size(), 1u);
  const auto& src = em.sources[0];
  EXPECT_EQ(src.name, "CAMS black carbon");
  EXPECT_EQ(src.mode, types::SourceMode::Offline);
  EXPECT_EQ(src.type, types::SourceType::Anthropogenic);
  EXPECT_EQ(src.inventory, "cams bc");
  EXPECT_EQ(src.species_map, "bc map");
  EXPECT_EQ(src.temporal_interpolation, types::TemporalInterpolation::Linear);
  EXPECT_EQ(src.vertical_injection, types::VerticalInjection::Surface);
  EXPECT_EQ(src.category, 0);
  EXPECT_EQ(src.hierarchy, 1);
  EXPECT_DOUBLE_EQ(src.scaling_factor, 1.0);
  EXPECT_EQ(src.sector, "anthropogenic");
  ASSERT_EQ(src.unknown_properties.size(), 1u);
  EXPECT_EQ(src.unknown_properties.at("__notes"), "test source");
}

TEST(EmissionsV1Parser, ParsesValidConfigFromString)
{
  const std::string content = R"(
version: 1.0.0
species: []
phases: []
reactions: []
emissions:
  inventories:
    - name: my inv
      directory: data
      file pattern: file_{YYYY}.nc
      convention: uptempo
  species maps:
    - name: my map
      mappings:
        - inventory species: SO2
          mechanism species: SO2
  sources:
    - name: so2 source
      mode: offline
      type: anthropogenic
      inventory: my inv
      species map: my map
      category: 0
      hierarchy: 1
)";

  auto result = ParseString(content);
  ASSERT_TRUE(result);
  ASSERT_TRUE(result->emissions.has_value());
  ASSERT_EQ(result->emissions->sources.size(), 1u);
  EXPECT_EQ(result->emissions->sources[0].name, "so2 source");
}

TEST(EmissionsV1Parser, MechanismWithoutEmissionsHasNoEmissions)
{
  const std::string content = R"(
version: 1.0.0
species: []
phases: []
reactions: []
)";
  auto result = ParseString(content);
  ASSERT_TRUE(result);
  EXPECT_FALSE(result->emissions.has_value());
}

TEST(EmissionsV1Parser, RejectsDuplicateSourceName)
{
  auto result = ParseFile("emissions_unit_configs/duplicate_source.yaml");
  ASSERT_FALSE(result);
  bool found = false;
  for (const auto& e : result.error())
    if (e.first == ErrorCode::DuplicateSourceDetected)
      found = true;
  EXPECT_TRUE(found);
}

TEST(EmissionsV1Parser, RejectsDuplicateCategoryHierarchy)
{
  auto result = ParseFile("emissions_unit_configs/duplicate_category_hierarchy.yaml");
  ASSERT_FALSE(result);
  bool found = false;
  for (const auto& e : result.error())
    if (e.first == ErrorCode::DuplicateCategoryHierarchy)
      found = true;
  EXPECT_TRUE(found);
}

TEST(EmissionsV1Parser, RejectsSourceWithUnknownInventory)
{
  auto result = ParseFile("emissions_unit_configs/unknown_inventory.yaml");
  ASSERT_FALSE(result);
  bool found = false;
  for (const auto& e : result.error())
    if (e.first == ErrorCode::SourceRequiresUnknownInventory)
      found = true;
  EXPECT_TRUE(found);
}

TEST(EmissionsV1Parser, RejectsSourceWithUnknownSpeciesMap)
{
  auto result = ParseFile("emissions_unit_configs/unknown_species_map.yaml");
  ASSERT_FALSE(result);
  bool found = false;
  for (const auto& e : result.error())
    if (e.first == ErrorCode::SourceRequiresUnknownSpeciesMap)
      found = true;
  EXPECT_TRUE(found);
}

TEST(EmissionsV1Parser, RejectsScalingFactorExceedingOne)
{
  auto result = ParseFile("emissions_unit_configs/scaling_exceeds_one.yaml");
  ASSERT_FALSE(result);
  bool found = false;
  for (const auto& e : result.error())
    if (e.first == ErrorCode::SpeciesMapScalingExceedsOne)
      found = true;
  EXPECT_TRUE(found);
}

TEST(EmissionsV1Parser, RejectsOnlineSource)
{
  auto result = ParseFile("emissions_unit_configs/online_source.yaml");
  ASSERT_FALSE(result);
  bool found = false;
  for (const auto& e : result.error())
    if (e.first == ErrorCode::OnlineSourcesNotSupported)
      found = true;
  EXPECT_TRUE(found);
}

TEST(EmissionsV1Parser, RejectsUnsupportedRegriddingType)
{
  auto result = ParseFile("emissions_unit_configs/unsupported_regridding.yaml");
  ASSERT_FALSE(result);
  bool found = false;
  for (const auto& e : result.error())
    if (e.first == ErrorCode::UnsupportedRegriddingType)
      found = true;
  EXPECT_TRUE(found);
}

TEST(EmissionsV1Parser, RejectsUnsupportedVerticalInjection)
{
  auto result = ParseFile("emissions_unit_configs/unsupported_vertical_injection.yaml");
  ASSERT_FALSE(result);
  bool found = false;
  for (const auto& e : result.error())
    if (e.first == ErrorCode::UnsupportedVerticalInjection)
      found = true;
  EXPECT_TRUE(found);
}

TEST(EmissionsV1Parser, AcceptsEmptySourcesList)
{
  const std::string content = R"(
version: 1.0.0
species: []
phases: []
reactions: []
emissions:
  sources: []
)";
  auto result = ParseString(content);
  ASSERT_TRUE(result);
  ASSERT_TRUE(result->emissions.has_value());
  EXPECT_TRUE(result->emissions->sources.empty());
}

TEST(EmissionsV1Parser, AcceptsAbsentOptionalSections)
{
  const std::string content = R"(
version: 1.0.0
species: []
phases: []
reactions: []
emissions: {}
)";
  auto result = ParseString(content);
  ASSERT_TRUE(result);
  ASSERT_TRUE(result->emissions.has_value());
  EXPECT_TRUE(result->emissions->sources.empty());
  EXPECT_TRUE(result->emissions->inventories.empty());
  EXPECT_TRUE(result->emissions->species_maps.empty());
}

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/emissions/v1/parse.hpp>
#include <mechanism_configuration/errors.hpp>

#include <gtest/gtest.h>

#include <filesystem>

using namespace mechanism_configuration;
namespace em = mechanism_configuration::emissions::v1;

// Wrapper so string literals route to the file-path overload without ambiguity.
static auto ParseFile(const char* p)
{
  return em::ParseEmissions(std::filesystem::path(p));
}
using em::ParseEmissions;

TEST(EmissionsV1Parser, ParsesValidConfig)
{
  auto result = ParseFile("emissions_unit_configs/valid.yaml");
  ASSERT_TRUE(result) << "Parse failed: " << (result ? "" : result.error()[0].second);

  const auto& config = *result;
  EXPECT_EQ(config.name, "Valid test emissions config");
  EXPECT_EQ(config.data_root, "/test/data");
  EXPECT_EQ(config.version.major, 1u);

  ASSERT_EQ(config.inventories.size(), 1u);
  ASSERT_TRUE(config.inventories.count("cams bc"));
  EXPECT_EQ(config.inventories.at("cams bc").directory, "cams");
  EXPECT_EQ(config.inventories.at("cams bc").file_pattern, "CAMS-GLOB-ANT_{YYYY}-{MM}.nc");
  EXPECT_EQ(config.inventories.at("cams bc").convention, "uptempo");

  ASSERT_EQ(config.species_maps.size(), 1u);
  ASSERT_TRUE(config.species_maps.count("bc map"));
  const auto& smap = config.species_maps.at("bc map");
  ASSERT_EQ(smap.mappings.size(), 1u);
  EXPECT_EQ(smap.mappings[0].inventory_species, "bc_anth_sum");
  EXPECT_EQ(smap.mappings[0].mechanism_species, "BC");
  EXPECT_DOUBLE_EQ(smap.mappings[0].scaling_factor, 1.0);

  EXPECT_EQ(config.regridding.type, em::types::RegriddingType::None);

  ASSERT_EQ(config.sources.size(), 1u);
  const auto& src = config.sources[0];
  EXPECT_EQ(src.name, "CAMS black carbon");
  EXPECT_EQ(src.mode, em::types::SourceMode::Offline);
  EXPECT_EQ(src.type, em::types::SourceType::Anthropogenic);
  EXPECT_EQ(src.inventory, "cams bc");
  EXPECT_EQ(src.species_map, "bc map");
  EXPECT_EQ(src.temporal_interpolation, em::types::TemporalInterpolation::Linear);
  EXPECT_EQ(src.vertical_injection, em::types::VerticalInjection::Surface);
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
kind: emissions
version: 1.0.0
name: In-memory test

inventories:
  my inv:
    directory: data
    file pattern: file_{YYYY}.nc
    convention: uptempo

species maps:
  my map:
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

  auto result = ParseEmissions(content);
  ASSERT_TRUE(result);
  EXPECT_EQ(result->name, "In-memory test");
  ASSERT_EQ(result->sources.size(), 1u);
  EXPECT_EQ(result->sources[0].name, "so2 source");
}

TEST(EmissionsV1Parser, RejectsWrongKind)
{
  auto result = ParseFile("emissions_unit_configs/wrong_kind.yaml");
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error()[0].first, ErrorCode::NotAnEmissionsConfig);
}

TEST(EmissionsV1Parser, RejectsMissingKind)
{
  auto result = ParseFile("emissions_unit_configs/missing_kind.yaml");
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error()[0].first, ErrorCode::NotAnEmissionsConfig);
}

TEST(EmissionsV1Parser, RejectsDuplicateSourceName)
{
  auto result = ParseFile("emissions_unit_configs/duplicate_source.yaml");
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error()[0].first, ErrorCode::DuplicateSourceDetected);
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
  EXPECT_EQ(result.error()[0].first, ErrorCode::SourceRequiresUnknownInventory);
}

TEST(EmissionsV1Parser, RejectsSourceWithUnknownSpeciesMap)
{
  auto result = ParseFile("emissions_unit_configs/unknown_species_map.yaml");
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error()[0].first, ErrorCode::SourceRequiresUnknownSpeciesMap);
}

TEST(EmissionsV1Parser, RejectsScalingFactorExceedingOne)
{
  auto result = ParseFile("emissions_unit_configs/scaling_exceeds_one.yaml");
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error()[0].first, ErrorCode::SpeciesMapScalingExceedsOne);
}

TEST(EmissionsV1Parser, RejectsOnlineSource)
{
  auto result = ParseFile("emissions_unit_configs/online_source.yaml");
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error()[0].first, ErrorCode::OnlineSourcesNotSupported);
}

TEST(EmissionsV1Parser, RejectsUnsupportedRegriddingType)
{
  auto result = ParseFile("emissions_unit_configs/unsupported_regridding.yaml");
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error()[0].first, ErrorCode::UnsupportedRegriddingType);
}

TEST(EmissionsV1Parser, RejectsUnsupportedVerticalInjection)
{
  auto result = ParseFile("emissions_unit_configs/unsupported_vertical_injection.yaml");
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error()[0].first, ErrorCode::UnsupportedVerticalInjection);
}

TEST(EmissionsV1Parser, AcceptsEmptySourcesList)
{
  const std::string content = R"(
kind: emissions
version: 1.0.0
sources: []
)";
  auto result = ParseEmissions(content);
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->sources.empty());
}

TEST(EmissionsV1Parser, AcceptsAbsentOptionalSections)
{
  const std::string content = R"(
kind: emissions
version: 1.0.0
)";
  auto result = ParseEmissions(content);
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->sources.empty());
  EXPECT_TRUE(result->inventories.empty());
  EXPECT_TRUE(result->species_maps.empty());
}

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/parse.hpp>

#include <gtest/gtest.h>

using namespace mechanism_configuration;

TEST(ParserBase, UnknownPropertiesKeepTypesAndStructure)
{
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = Parse(std::string("v1_unit_configs/unknown_properties/valid") + extension);
    ASSERT_TRUE(parsed) << extension;
    Mechanism mechanism = *parsed;

    // Top-level properties
    EXPECT_EQ(mechanism.unknown_properties.size(), 3);
    EXPECT_EQ(mechanism.unknown_properties.at("__author"), "someone");
    ASSERT_TRUE(mechanism.unknown_properties.at("__revision").IsInt());
    EXPECT_EQ(mechanism.unknown_properties.at("__revision").AsInt(), 3);
    const auto& cam_chem = mechanism.unknown_properties.at("__cam_chem");
    ASSERT_TRUE(cam_chem.IsObject());
    ASSERT_TRUE(cam_chem.AsObject().at("fixed").IsArray());
    EXPECT_EQ(cam_chem.AsObject().at("fixed").AsArray().at(0), "CH4");
    EXPECT_EQ(cam_chem.AsObject().at("ext_forcing").AsObject().at("SO2"), "Forced");
    EXPECT_EQ(cam_chem.ToJson(), R"({"ext_forcing":{"SO2":"Forced"},"fixed":["CH4"]})");

    // Scalar types on a species
    const auto& species = mechanism.species.at(0).unknown_properties;
    EXPECT_EQ(species.at("__quoted number"), "0.5");
    ASSERT_TRUE(species.at("__plain number").IsDouble());
    EXPECT_EQ(species.at("__plain number").AsDouble(), 0.5);
    ASSERT_TRUE(species.at("__flag").IsBool());
    EXPECT_TRUE(species.at("__flag").AsBool());
    EXPECT_TRUE(species.at("__nothing").IsNull());
    // Too large for a 64-bit integer, so it becomes a double
    ASSERT_TRUE(species.at("__big").IsDouble());
    EXPECT_DOUBLE_EQ(species.at("__big").AsDouble(), 1e20);
    EXPECT_TRUE(mechanism.species.at(1).unknown_properties.empty());

    // Phase
    const auto& kpp = mechanism.phases.at(0).unknown_properties.at("__kpp");
    EXPECT_EQ(kpp.AsObject().at("integrator"), "rosenbrock");

    // Reaction and reaction component
    const auto& reaction = mechanism.reactions.arrhenius.at(0);
    EXPECT_EQ(reaction.unknown_properties.at("__wrf_chem").AsObject().at("custom_rate"), "usr_CO_OH_a");
    const auto& tags = reaction.reactants.at(0).unknown_properties.at("__tags");
    ASSERT_TRUE(tags.IsArray());
    EXPECT_EQ(tags.AsArray().size(), 3);
    EXPECT_EQ(tags.AsArray()[0].AsInt(), 1);
    EXPECT_EQ(tags.AsArray()[1], "two");
    EXPECT_TRUE(tags.AsArray()[2].IsDouble());
    EXPECT_EQ(tags.ToJson(), R"([1,"two",3.0])");
  }
}

TEST(PropertyValue, ToJsonEscapesStrings)
{
  types::PropertyValue value(std::string("line\n\"quoted\""));
  EXPECT_EQ(value.ToJson(), R"("line\n\"quoted\"")");
  EXPECT_EQ(types::PropertyValue(0.1).ToJson(), "0.1");
  EXPECT_EQ(types::PropertyValue(nullptr).ToJson(), "null");
}

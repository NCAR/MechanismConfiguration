// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/types.hpp>
#include <mechanism_configuration/mechanism.hpp>

#include <yaml-cpp/yaml.h>

#include <vector>

namespace mechanism_configuration::v0
{
  // species and mechanism
  Errors ParseChemicalSpecies(Mechanism& mechanism, const YAML::Node& object);
  Errors ParseProducts(const YAML::Node& object, std::vector<types::ReactionComponent>& products);
  Errors ParseReactants(const YAML::Node& object, std::vector<types::ReactionComponent>& reactants);
  Errors ParseRelativeTolerance(Mechanism& mechanism, const YAML::Node& object);

  // reactions
  Errors ArrheniusParser(Mechanism& mechanism, const YAML::Node& object);
  Errors BranchedParser(Mechanism& mechanism, const YAML::Node& object);
  Errors EmissionParser(Mechanism& mechanism, const YAML::Node& object);
  Errors FirstOrderLossParser(Mechanism& mechanism, const YAML::Node& object);
  Errors PhotolysisParser(Mechanism& mechanism, const YAML::Node& object);
  Errors SurfaceParser(Mechanism& mechanism, const YAML::Node& object);
  Errors TernaryChemicalActivationParser(Mechanism& mechanism, const YAML::Node& object);
  Errors TroeParser(Mechanism& mechanism, const YAML::Node& object);
  Errors TunnelingParser(Mechanism& mechanism, const YAML::Node& object);
  Errors UserDefinedParser(Mechanism& mechanism, const YAML::Node& object);
}  // namespace mechanism_configuration::v0

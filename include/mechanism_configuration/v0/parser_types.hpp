// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/parse_status.hpp>
#include <mechanism_configuration/v0/types.hpp>

#include <yaml-cpp/yaml.h>

#include <vector>

namespace mechanism_configuration
{
  namespace v0
  {
    // species and mechanism
    Errors ParseChemicalSpecies(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    Errors ParseProducts(const YAML::Node& object, std::vector<types::ReactionComponent>& products);
    Errors ParseReactants(const YAML::Node& object, std::vector<types::ReactionComponent>& reactants);
    Errors ParseRelativeTolerance(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);

    // reactions
    Errors ArrheniusParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    Errors BranchedParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    Errors EmissionParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    Errors FirstOrderLossParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    Errors PhotolysisParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    Errors SurfaceParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    Errors TernaryChemicalActivationParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    Errors TroeParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    Errors TunnelingParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    Errors UserDefinedParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);

  }  // namespace v0
}  // namespace mechanism_configuration

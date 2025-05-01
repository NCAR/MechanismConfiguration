// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/parse_status.hpp>
#include <mechanism_configuration/v1/types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <vector>

namespace mechanism_configuration
{
  namespace v1
  {
    std::pair<Errors, std::vector<v1::types::Species>> ParseSpecies(const YAML::Node& objects);

    class CondensedPhaseArrheniusParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class TroeParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class BranchedParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class TunnelingParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class SurfaceParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class CondensedPhasePhotolysisParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class SimpolPhaseTransferParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class EmissionParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class PhotolysisParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class FirstOrderLossParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class AqueousEquilibriumParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class WetDepositionParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class HenrysLawParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class ArrheniusParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };

    class UserDefinedParser : public IReactionParser
    {
     public:
      Errors parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) override;
    };
  }  // namespace v1
}  // namespace mechanism_configuration

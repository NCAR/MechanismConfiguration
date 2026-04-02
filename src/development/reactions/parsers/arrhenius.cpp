// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/type_parsers.hpp>
#include <mechanism_configuration/development/types.hpp>
#include <mechanism_configuration/development/utils.hpp>

namespace mechanism_configuration
{
  namespace development
  {
    /// @brief Parses a YAML-defined Arrhenius reaction and appends it to the reaction list.
    ///        Extracts reactants, products, kinetic parameters (A–E, Ea), gas phase,
    ///        optional metadata (name, comments), and constructs a `types::Arrhenius` object.
    /// @param object The YAML node representing the reaction
    /// @param reactions The reactions container to append the parsed reaction to
    void ArrheniusParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Arrhenius arrhenius;

      arrhenius.gas_phase = object[validation::gas_phase].as<std::string>();
      arrhenius.reactants = ParseReactionComponents(object, validation::reactants);
      arrhenius.products = ParseReactionComponents(object, validation::products);
      arrhenius.unknown_properties = GetComments(object);

      if (object[validation::A])
      {
        arrhenius.A = object[validation::A].as<double>();
      }
      if (object[validation::B])
      {
        arrhenius.B = object[validation::B].as<double>();
      }
      if (object[validation::C])
      {
        arrhenius.C = object[validation::C].as<double>();
      }
      if (object[validation::D])
      {
        arrhenius.D = object[validation::D].as<double>();
      }
      if (object[validation::E])
      {
        arrhenius.E = object[validation::E].as<double>();
      }
      if (object[validation::Ea])
      {
        arrhenius.C = -1 * object[validation::Ea].as<double>() / constants::boltzmann;
      }
      if (object[validation::name])
      {
        arrhenius.name = object[validation::name].as<std::string>();
      }

      reactions.arrhenius.emplace_back(std::move(arrhenius));
    }

  }  // namespace development
}  // namespace mechanism_configuration

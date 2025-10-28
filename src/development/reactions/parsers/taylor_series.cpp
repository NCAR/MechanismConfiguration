// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/development/mechanism_parsers.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/utils.hpp>

namespace mechanism_configuration
{
  namespace development
  {
    /// @brief Parses a YAML-defined Taylor Series reaction and appends it to the reaction list.
    ///        Extracts reactants, products, kinetic parameters (A–E, Ea), gas phase,
    ///        optional metadata (name, comments), and constructs a `types::TaylorSeries` object.
    /// @param object The YAML node representing the reaction
    /// @param reactions The reactions container to append the parsed reaction to
    void TaylorSeriesParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::TaylorSeries taylor_series;

      for (const auto& elem : object[validation::reactants])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient])
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        taylor_series.reactants.emplace_back(std::move(component));
      }

      for (const auto& elem : object[validation::products])
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);
        if (elem[validation::coefficient])
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        taylor_series.products.emplace_back(std::move(component));
      }

      if (object[validation::A])
      {
        taylor_series.A = object[validation::A].as<double>();
      }
      if (object[validation::B])
      {
        taylor_series.B = object[validation::B].as<double>();
      }
      if (object[validation::C])
      {
        taylor_series.C = object[validation::C].as<double>();
      }
      if (object[validation::D])
      {
        taylor_series.D = object[validation::D].as<double>();
      }
      if (object[validation::E])
      {
        taylor_series.E = object[validation::E].as<double>();
      }
      if (object[validation::Ea])
      {
        taylor_series.C = -1 * object[validation::Ea].as<double>() / constants::boltzmann;
      }
      if (object[validation::taylor_coefficients])
      {
        taylor_series.taylor_coefficients = object[validation::taylor_coefficients].as<std::vector<double>>();
      }

      if (object[validation::name])
      {
        taylor_series.name = object[validation::name].as<std::string>();
      }

      taylor_series.gas_phase = object[validation::gas_phase].as<std::string>();
      taylor_series.unknown_properties = GetComments(object);

      reactions.taylor_series.emplace_back(std::move(taylor_series));
    }

  }  // namespace development
}  // namespace mechanism_configuration

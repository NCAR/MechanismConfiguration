// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/types/reactions.hpp>
#include <mechanism_configuration/types/species.hpp>

#include <detail/constants.hpp>
#include <detail/error_format.hpp>
#include <detail/schema.hpp>
#include <detail/v1/keys.hpp>
#include <detail/v1/reactions/parsers.hpp>
#include <detail/v1/reactions/schema.hpp>
#include <detail/v1/utils.hpp>

namespace mechanism_configuration::v1
{
  /// @brief Checks the structural schema of a YAML-defined Taylor Series reaction entry
  ///        Performs schema validation, checks for mutually exclusive parameters (`Ea` vs `C`),
  ///        and collects any structural errors found.
  /// @param object The YAML node representing the reaction
  /// @param existing_species Unused; semantic checks live in ValidateSemantics
  /// @param existing_phases Unused; semantic checks live in ValidateSemantics
  /// @return A list of validation errors, if any
  Errors TaylorSeriesParser::CheckSchema(
      const YAML::Node& object,
      const std::vector<types::Species>& existing_species,
      const std::vector<types::Phase>& existing_phases)
  {
    std::vector<std::string_view> required_keys = { keys::reactants, keys::products, keys::type, keys::gas_phase };
    std::vector<std::string_view> optional_keys = { keys::A, keys::B,  keys::C,    keys::D,
                                                    keys::E, keys::Ea, keys::name, keys::taylor_coefficients };
    Errors errors;

    auto schema_errors = mechanism_configuration::CheckSchema(object, required_keys, optional_keys);
    if (!schema_errors.empty())
    {
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      return errors;
    }

    // Reactants
    schema_errors = CheckReactantsOrProductsSchema(object[keys::reactants]);
    if (!schema_errors.empty())
    {
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
    }

    // Products
    schema_errors = CheckReactantsOrProductsSchema(object[keys::products]);
    if (!schema_errors.empty())
    {
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
    }

    if (object[keys::Ea] && object[keys::C])
    {
      const auto& node = object[keys::Ea];
      ErrorLocation error_location{ node.Mark().line, node.Mark().column };

      std::string message = mc_fmt::format(
          "{} error: Mutually exclusive option of 'Ea' and 'C' found in '{}' reaction.",
          error_location,
          object[keys::type].as<std::string>());

      errors.push_back({ ErrorCode::MutuallyExclusiveOption, message });
    }

    // Semantic checks are performed by the version-neutral ValidateSemantics.

    return errors;
  }

  /// @brief Parses a YAML-defined Taylor Series reaction and appends it to the reaction list.
  ///        Extracts reactants, products, kinetic parameters (A–E, Ea), gas phase,
  ///        optional metadata (name, comments), and constructs a `types::TaylorSeries` object.
  /// @param object The YAML node representing the reaction
  /// @param reactions The reactions container to append the parsed reaction to
  void TaylorSeriesParser::Parse(const YAML::Node& object, types::Reactions& reactions)
  {
    types::TaylorSeries taylor_series;

    taylor_series.gas_phase = object[keys::gas_phase].as<std::string>();
    taylor_series.reactants = ParseReactionComponents(object, keys::reactants);
    taylor_series.products = ParseReactionComponents(object, keys::products);
    taylor_series.unknown_properties = GetComments(object);

    if (object[keys::A])
    {
      taylor_series.A = object[keys::A].as<double>();
    }
    if (object[keys::B])
    {
      taylor_series.B = object[keys::B].as<double>();
    }
    if (object[keys::C])
    {
      taylor_series.C = object[keys::C].as<double>();
    }
    if (object[keys::D])
    {
      taylor_series.D = object[keys::D].as<double>();
    }
    if (object[keys::E])
    {
      taylor_series.E = object[keys::E].as<double>();
    }
    if (object[keys::Ea])
    {
      taylor_series.C = -1 * object[keys::Ea].as<double>() / constants::BOLTZMANN;
    }
    if (object[keys::taylor_coefficients])
    {
      taylor_series.taylor_coefficients = object[keys::taylor_coefficients].as<std::vector<double>>();
    }
    if (object[keys::name])
    {
      taylor_series.name = object[keys::name].as<std::string>();
    }

    reactions.taylor_series.emplace_back(std::move(taylor_series));
  }

}  // namespace mechanism_configuration::v1

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <detail/constants.hpp>
#include <detail/v1/reaction_parsers.hpp>
#include <mechanism_configuration/types.hpp>
#include <detail/v1/type_parsers.hpp>
#include <detail/v1/type_schema.hpp>
#include <detail/v1/utils.hpp>
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <detail/check_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
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
      std::vector<std::string_view> required_keys = {
        validation::reactants, validation::products, validation::type, validation::gas_phase
      };
      std::vector<std::string_view> optional_keys = { validation::A,    validation::B,
                                                 validation::C,    validation::D,
                                                 validation::E,    validation::Ea,
                                                 validation::name, validation::taylor_coefficients };
      Errors errors;

      auto schema_errors = mechanism_configuration::CheckSchema(object, required_keys, optional_keys);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
        return errors;
      }

      // Reactants
      schema_errors = CheckReactantsOrProductsSchema(object[validation::reactants]);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }

      // Products
      schema_errors = CheckReactantsOrProductsSchema(object[validation::products]);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }

      if (object[validation::Ea] && object[validation::C])
      {
        const auto& node = object[validation::Ea];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string message = mc_fmt::format(
            "{} error: Mutually exclusive option of 'Ea' and 'C' found in '{}' reaction.",
            error_location,
            object[validation::type].as<std::string>());

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

      taylor_series.gas_phase = object[validation::gas_phase].as<std::string>();
      taylor_series.reactants = ParseReactionComponents(object, validation::reactants);
      taylor_series.products = ParseReactionComponents(object, validation::products);
      taylor_series.unknown_properties = GetComments(object);

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

      reactions.taylor_series.emplace_back(std::move(taylor_series));
    }

  }  // namespace v1
}  // namespace mechanism_configuration

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/types/reactions.hpp>
#include <mechanism_configuration/types/species.hpp>

#include <detail/check_schema.hpp>
#include <detail/constants.hpp>
#include <detail/error_format.hpp>
#include <detail/v1/reaction_parsers.hpp>
#include <detail/v1/type_parsers.hpp>
#include <detail/v1/type_schema.hpp>
#include <detail/v1/utils.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    /// @brief Checks the structural schema of a YAML-defined Arrhenius reaction entry
    ///        Performs schema validation, checks for mutually exclusive parameters (`Ea` vs `C`),
    ///        and collects any structural errors found.
    /// @param object The YAML node representing the reaction
    /// @param existing_species Unused; semantic checks live in ValidateSemantics
    /// @param existing_phases Unused; semantic checks live in ValidateSemantics
    /// @return A list of validation errors, if any
    Errors ArrheniusParser::CheckSchema(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      std::vector<std::string_view> required_keys = { keys::reactants, keys::products, keys::type, keys::gas_phase };
      std::vector<std::string_view> optional_keys = { keys::A, keys::B, keys::C, keys::D, keys::E, keys::Ea, keys::name };
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

      // Semantic checks (species existence, phase membership) are performed by the
      // version-neutral ValidateSemantics over the canonical Mechanism.
      return errors;
    }

    /// @brief Parses a YAML-defined Arrhenius reaction and appends it to the reaction list.
    ///        Extracts reactants, products, kinetic parameters (A–E, Ea), gas phase,
    ///        optional metadata (name, comments), and constructs a `types::Arrhenius` object.
    /// @param object The YAML node representing the reaction
    /// @param reactions The reactions container to append the parsed reaction to
    void ArrheniusParser::Parse(const YAML::Node& object, types::Reactions& reactions)
    {
      types::Arrhenius arrhenius;

      arrhenius.gas_phase = object[keys::gas_phase].as<std::string>();
      arrhenius.reactants = ParseReactionComponents(object, keys::reactants);
      arrhenius.products = ParseReactionComponents(object, keys::products);
      arrhenius.unknown_properties = GetComments(object);

      if (object[keys::A])
      {
        arrhenius.A = object[keys::A].as<double>();
      }
      if (object[keys::B])
      {
        arrhenius.B = object[keys::B].as<double>();
      }
      if (object[keys::C])
      {
        arrhenius.C = object[keys::C].as<double>();
      }
      if (object[keys::D])
      {
        arrhenius.D = object[keys::D].as<double>();
      }
      if (object[keys::E])
      {
        arrhenius.E = object[keys::E].as<double>();
      }
      if (object[keys::Ea])
      {
        arrhenius.C = -1 * object[keys::Ea].as<double>() / constants::BOLTZMANN;
      }
      if (object[keys::name])
      {
        arrhenius.name = object[keys::name].as<std::string>();
      }

      reactions.arrhenius.emplace_back(std::move(arrhenius));
    }

  }  // namespace v1
}  // namespace mechanism_configuration

// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/mechanism_parsers.hpp>
#include <mechanism_configuration/v1/reaction_parsers.hpp>
#include <mechanism_configuration/v1/reaction_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors BranchedParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::Branched branched;

      std::vector<std::string> required_keys = { validation::nitrate_products,
                                                 validation::alkoxy_products,
                                                 validation::reactants,
                                                 validation::type,
                                                 validation::gas_phase };
      std::vector<std::string> optional_keys = {
        validation::name, validation::X, validation::Y, validation::a0, validation::n
      };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        auto alkoxy_products = ParseReactantsOrProducts(validation::alkoxy_products, object);
        errors.insert(errors.end(), alkoxy_products.first.begin(), alkoxy_products.first.end());
        auto nitrate_products = ParseReactantsOrProducts(validation::nitrate_products, object);
        errors.insert(errors.end(), nitrate_products.first.begin(), nitrate_products.first.end());
        auto reactants = ParseReactantsOrProducts(validation::reactants, object);
        errors.insert(errors.end(), reactants.first.begin(), reactants.first.end());

        branched.X = object[validation::X].as<double>();
        branched.Y = object[validation::Y].as<double>();
        branched.a0 = object[validation::a0].as<double>();
        branched.n = object[validation::n].as<double>();

        if (object[validation::name])
        {
          branched.name = object[validation::name].as<std::string>();
        }

        std::vector<std::string> requested_species;
        for (const auto& spec : nitrate_products.second)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : alkoxy_products.second)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants.second)
        {
          requested_species.push_back(spec.species_name);
        }

        std::vector<std::string> unknown_species = FindUnknownSpecies(requested_species, existing_species);
        if (!unknown_species.empty())
        {
          std::ostringstream oss;

          std::string line = std::to_string(object.Mark().line + 1);
          std::string column = std::to_string(object.Mark().column + 1);
          oss << line << ":" << column;

          if (object[validation::name])
          {
            oss << " error: Reaction '" << object[validation::name].as<std::string>() << "' requires unknown species: ";
          }
          else
          {
            oss << " error: Reaction requires unknown species: ";
          }

          for (size_t i = 0; i < unknown_species.size(); i++)
          {
            oss << "'" << unknown_species[i] << "'";
            if (i != unknown_species.size() - 1)
            {
              oss << ", ";
            }
          }

          errors.push_back({ ConfigParseStatus::ReactionRequiresUnknownSpecies, oss.str() });
        }

        std::string gas_phase = object[validation::gas_phase].as<std::string>();
        auto it = std::find_if(
            existing_phases.begin(),
            existing_phases.end(),
            [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::gas_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::gas_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + gas_phase });
        }

        branched.gas_phase = gas_phase;
        branched.nitrate_products = nitrate_products.second;
        branched.alkoxy_products = alkoxy_products.second;
        branched.reactants = reactants.second;
        branched.unknown_properties = GetComments(object);
        reactions.branched.push_back(branched);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

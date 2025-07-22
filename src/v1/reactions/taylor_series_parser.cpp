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
    Errors TaylorSeriesParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::TaylorSeries taylor_series;

      std::vector<std::string> required_keys = { validation::products, validation::reactants, validation::type, validation::gas_phase };
      std::vector<std::string> optional_keys = {
        validation::A, validation::B, validation::C, validation::D, validation::E, validation::Ea, validation::taylor_coefficients, validation::name
      };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        auto products = ParseReactantsOrProducts(validation::products, object);
        errors.insert(errors.end(), products.first.begin(), products.first.end());
        auto reactants = ParseReactantsOrProducts(validation::reactants, object);
        errors.insert(errors.end(), reactants.first.begin(), reactants.first.end());

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
          if (taylor_series.C != 0)
          {
            std::string line = std::to_string(object[validation::Ea].Mark().line + 1);
            std::string column = std::to_string(object[validation::Ea].Mark().column + 1);
            errors.push_back({ ConfigParseStatus::MutuallyExclusiveOption, line + ":" + column + ": Mutually exclusive option: Ea and C" });
          }
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

        std::vector<std::string> requested_species;
        for (const auto& spec : products.second)
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
        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::gas_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::gas_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + gas_phase });
        }

        taylor_series.gas_phase = gas_phase;
        taylor_series.products = products.second;
        taylor_series.reactants = reactants.second;
        taylor_series.unknown_properties = GetComments(object);
        reactions.taylor_series.push_back(taylor_series);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

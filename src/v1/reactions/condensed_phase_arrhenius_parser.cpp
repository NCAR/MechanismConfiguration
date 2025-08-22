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
    Errors CondensedPhaseArrheniusParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::CondensedPhaseArrhenius condensed_phase_arrhenius;

      std::vector<std::string> required_keys = { validation::products, validation::reactants, validation::type, validation::condensed_phase };
      std::vector<std::string> optional_keys = { validation::A, validation::B,  validation::C,   validation::D,
                                                 validation::E, validation::Ea, validation::name };

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
          condensed_phase_arrhenius.A = object[validation::A].as<double>();
        }
        if (object[validation::B])
        {
          condensed_phase_arrhenius.B = object[validation::B].as<double>();
        }
        if (object[validation::C])
        {
          condensed_phase_arrhenius.C = object[validation::C].as<double>();
        }
        if (object[validation::D])
        {
          condensed_phase_arrhenius.D = object[validation::D].as<double>();
        }
        if (object[validation::E])
        {
          condensed_phase_arrhenius.E = object[validation::E].as<double>();
        }
        if (object[validation::Ea])
        {
          if (condensed_phase_arrhenius.C != 0)
          {
            std::string line = std::to_string(object[validation::Ea].Mark().line + 1);
            std::string column = std::to_string(object[validation::Ea].Mark().column + 1);
            errors.push_back({ ConfigParseStatus::MutuallyExclusiveOption, line + ":" + column + ": Cannot specify both 'C' and 'Ea'" });
          }
          condensed_phase_arrhenius.C = -1 * object[validation::Ea].as<double>() / constants::boltzmann;
        }

        if (object[validation::name])
        {
          condensed_phase_arrhenius.name = object[validation::name].as<std::string>();
        }

        std::string condensed_phase = object[validation::condensed_phase].as<std::string>();

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

        auto phase_it = std::find_if(
            existing_phases.begin(), existing_phases.end(), [&condensed_phase](const types::Phase& phase) { return phase.name == condensed_phase; });

        if (phase_it != existing_phases.end())
        {
          std::vector<std::string> condensed_phase_species = GetSpeciesNames((*phase_it).species);
          std::vector<std::string> unknown_species = FindUnknownSpecies(requested_species, condensed_phase_species);
          if (!unknown_species.empty())
          {
            std::ostringstream oss;

            std::string line = std::to_string(object.Mark().line + 1);
            std::string column = std::to_string(object.Mark().column + 1);
            oss << line << ":" << column;
            oss << " error: '" << phase_it->name << "' phase requires unknown '" << phase_it->name << "' species: ";

            for (size_t i = 0; i < unknown_species.size(); i++)
            {
              oss << "'" << unknown_species[i] << "'";
              if (i != unknown_species.size() - 1)
              {
                oss << ", ";
              }
            }

            errors.push_back({ ConfigParseStatus::PhaseRequiresUnknownSpecies, oss.str() });
          }
        }
        else
        {
          std::string line = std::to_string(object.Mark().line + 1);
          std::string column = std::to_string(object.Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + condensed_phase });
        }

        condensed_phase_arrhenius.condensed_phase = condensed_phase;
        condensed_phase_arrhenius.products = products.second;
        condensed_phase_arrhenius.reactants = reactants.second;
        condensed_phase_arrhenius.unknown_properties = GetComments(object);
        reactions.condensed_phase_arrhenius.push_back(condensed_phase_arrhenius);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

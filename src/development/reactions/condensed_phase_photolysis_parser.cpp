// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/development/mechanism_parsers.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>
#include <mechanism_configuration/development/reaction_types.hpp>
#include <mechanism_configuration/development/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace development
  {
    Errors CondensedPhasePhotolysisParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::CondensedPhasePhotolysis condensed_phase_photolysis;

      std::vector<std::string> required_keys = { validation::reactants, validation::products, validation::type, validation::condensed_phase };
      std::vector<std::string> optional_keys = { validation::name, validation::scaling_factor };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        auto products = ParseReactantsOrProducts(validation::products, object);
        errors.insert(errors.end(), products.first.begin(), products.first.end());
        auto reactants = ParseReactantsOrProducts(validation::reactants, object);
        errors.insert(errors.end(), reactants.first.begin(), reactants.first.end());

        if (object[validation::scaling_factor])
        {
          condensed_phase_photolysis.scaling_factor = object[validation::scaling_factor].as<double>();
        }

        if (object[validation::name])
        {
          condensed_phase_photolysis.name = object[validation::name].as<std::string>();
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

        if (reactants.second.size() > 1)
        {
          std::string line = std::to_string(object[validation::reactants].Mark().line + 1);
          std::string column = std::to_string(object[validation::reactants].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::TooManyReactionComponents, line + ":" + column + ": Too many reaction components" });
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

        condensed_phase_photolysis.condensed_phase = condensed_phase;
        condensed_phase_photolysis.products = products.second;
        condensed_phase_photolysis.reactants = reactants.second;
        condensed_phase_photolysis.unknown_properties = GetComments(object);
        reactions.condensed_phase_photolysis.push_back(condensed_phase_photolysis);
      }

      return errors;
    }
  }  // namespace development
}  // namespace mechanism_configuration

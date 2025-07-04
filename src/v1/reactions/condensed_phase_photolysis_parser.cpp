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
    Errors CondensedPhasePhotolysisParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::CondensedPhasePhotolysis condensed_phase_photolysis;

      std::vector<std::string> required_keys = { validation::reactants, validation::products, validation::type, validation::aqueous_phase };
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

        std::string aqueous_phase = object[validation::aqueous_phase].as<std::string>();

        std::vector<std::string> requested_species;
        for (const auto& spec : products.second)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants.second)
        {
          requested_species.push_back(spec.species_name);
        }

        if (RequiresUnknownSpecies(requested_species, existing_species))
        {
          std::string line = std::to_string(object.Mark().line + 1);
          std::string column = std::to_string(object.Mark().column + 1);
          errors.push_back({ ConfigParseStatus::ReactionRequiresUnknownSpecies, line + ":" + column + ": Reaction requires unknown species" });
        }

        if (reactants.second.size() > 1)
        {
          std::string line = std::to_string(object[validation::reactants].Mark().line + 1);
          std::string column = std::to_string(object[validation::reactants].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::TooManyReactionComponents, line + ":" + column + ": Too many reaction components" });
        }

        auto phase_it = std::find_if(
            existing_phases.begin(), existing_phases.end(), [&aqueous_phase](const types::Phase& phase) { return phase.name == aqueous_phase; });

        if (phase_it != existing_phases.end())
        {
          std::vector<std::string> aqueous_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
          if (RequiresUnknownSpecies(requested_species, aqueous_phase_species))
          {
            std::string line = std::to_string(object.Mark().line + 1);
            std::string column = std::to_string(object.Mark().column + 1);
            errors.push_back({ ConfigParseStatus::RequestedAqueousSpeciesNotIncludedInAqueousPhase,
                               line + ":" + column + ": Requested aqueous species not included in aqueous phase" });
          }
        }
        else
        {
          std::string line = std::to_string(object.Mark().line + 1);
          std::string column = std::to_string(object.Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + aqueous_phase });
        }

        condensed_phase_photolysis.aqueous_phase = aqueous_phase;
        condensed_phase_photolysis.products = products.second;
        condensed_phase_photolysis.reactants = reactants.second;
        condensed_phase_photolysis.unknown_properties = GetComments(object);
        reactions.condensed_phase_photolysis.push_back(condensed_phase_photolysis);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

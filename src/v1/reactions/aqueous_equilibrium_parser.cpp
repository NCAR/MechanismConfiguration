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
    Errors AqueousEquilibriumParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::AqueousEquilibrium aqueous_equilibrium;

      std::vector<std::string> required_keys = {
        validation::type, validation::reactants, validation::products, validation::aqueous_phase, validation::k_reverse
      };
      std::vector<std::string> optional_keys = { validation::name, validation::A, validation::C };

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
          aqueous_equilibrium.A = object[validation::A].as<double>();
        }
        if (object[validation::C])
        {
          aqueous_equilibrium.C = object[validation::C].as<double>();
        }

        aqueous_equilibrium.k_reverse = object[validation::k_reverse].as<double>();

        if (object[validation::name])
        {
          aqueous_equilibrium.name = object[validation::name].as<std::string>();
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

        aqueous_equilibrium.aqueous_phase = aqueous_phase;
        aqueous_equilibrium.products = products.second;
        aqueous_equilibrium.reactants = reactants.second;
        aqueous_equilibrium.unknown_properties = GetComments(object);
        reactions.aqueous_equilibrium.push_back(aqueous_equilibrium);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

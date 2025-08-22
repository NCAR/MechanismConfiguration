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
    Errors HenrysLawParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::HenrysLaw henrys_law;

      std::vector<std::string> required_top_level_keys = { validation::type, validation::gas, validation::particle };
      std::vector<std::string> optional_top_level_keys = { validation::name };

      std::vector<std::string> required_second_level_keys_for_gas = { validation::name, validation::species };
      std::vector<std::string> required_second_level_keys_for_particle = { validation::phase, validation::solutes, validation::solvent };
      std::vector<std::string> optional_second_level_keys = {};

      auto has_error_top_level_keys = ValidateSchema(object, required_top_level_keys, optional_top_level_keys);
      auto has_error_second_level_keys_for_gas =
          ValidateSchema(object[validation::gas], required_second_level_keys_for_gas, optional_second_level_keys);
      auto has_error_second_level_keys_for_particle =
          ValidateSchema(object[validation::particle], required_second_level_keys_for_particle, optional_second_level_keys);

      errors.insert(errors.end(), has_error_top_level_keys.begin(), has_error_top_level_keys.end());
      errors.insert(errors.end(), has_error_second_level_keys_for_gas.begin(), has_error_second_level_keys_for_gas.end());
      errors.insert(errors.end(), has_error_second_level_keys_for_particle.begin(), has_error_second_level_keys_for_particle.end());

      if (has_error_top_level_keys.empty() && has_error_second_level_keys_for_gas.empty() && has_error_second_level_keys_for_particle.empty())
      {
        types::Phase gas;
        gas.name = object[validation::gas][validation::name].as<std::string>();
        for (const auto& elem : object[validation::gas][validation::species])
        {
          types::PhaseSpecies phase_species;
          phase_species.name = elem.as<std::string>();
          gas.species.push_back(phase_species);
        }

        types::Particle particle;
        particle.phase = object[validation::particle][validation::phase].as<std::string>();
        for (const auto& elem : object[validation::particle][validation::solutes])
        {
          types::ReactionComponent solute;
          solute.species_name = elem.as<std::string>();
          particle.solutes.push_back(solute);
        }
        types::ReactionComponent solvent_from_config;
        solvent_from_config.species_name = object[validation::particle][validation::solvent].as<std::string>();
        particle.solvent = solvent_from_config;

        if (object[validation::name])
        {
          henrys_law.name = object[validation::name].as<std::string>();
        }

        // Check whether the species listed in the reactions are valid by
        // comparing them to the registered species
        std::vector<std::string> combined_species;
        combined_species.insert(combined_species.end(), GetSpeciesNames(gas.species).begin(), GetSpeciesNames(gas.species).end());
        for (const auto& solute : particle.solutes)
        {
          combined_species.push_back(solute.species_name);
        }
        combined_species.push_back(particle.solvent.species_name);

        std::vector<std::string> unknown_species = FindUnknownSpecies(combined_species, existing_species);
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

        // Check whether the phases in the reactions are valid by comparing them to the initialized phases
        auto it_found_gas_phase =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&gas](const auto& phase) { return phase.name == gas.name; });
        if (it_found_gas_phase == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::gas][validation::name].Mark().line + 1);
          std::string column = std::to_string(object[validation::gas][validation::name].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + gas.name });
        }
        // Check whether the species belong to the corresponding phase in that reactions (mulitple phases)
        else
        {
          std::vector<std::string> species_registered_in_phase = GetSpeciesNames(it_found_gas_phase->species);
          std::vector<std::string> unknown_species_gas = FindUnknownSpecies(GetSpeciesNames(gas.species), species_registered_in_phase);
          if (!unknown_species_gas.empty())
          {
            std::ostringstream oss;

            std::string line = std::to_string(object.Mark().line + 1);
            std::string column = std::to_string(object.Mark().column + 1);
            oss << line << ":" << column;
            oss << " error: Required '" << gas.name << "' species ";

            for (size_t i = 0; i < unknown_species_gas.size(); i++)
            {
              oss << "'" << unknown_species_gas[i] << "'";
              if (i != unknown_species_gas.size() - 1)
              {
                oss << ", ";
              }
            }

            if (object[validation::name])
            {
              oss << " in '" << object[validation::name].as<std::string>() << "' reaction";
            }
            else
            {
              oss << " in the reaction";
            }
            oss << " do not exist in the species registered in the '" << gas.name << "' phase";

            errors.push_back({ ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase, oss.str() });
          }
        }

        // Check whether the phases in the reactions are valid by comparing them to the initialized phases
        auto it_found_particle_phase =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&particle](const auto& phase) { return phase.name == particle.phase; });
        if (it_found_gas_phase == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::particle][validation::phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::particle][validation::phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + particle.phase });
        }
        // Check whether the species in solutes belong to the corresponding phase
        else
        {
          std::vector<std::string> species_registered_in_phase = GetSpeciesNames(it_found_particle_phase->species);
          std::vector<std::string> speices_in_solutes;
          for (const auto& elem : particle.solutes)
          {
            speices_in_solutes.push_back(elem.species_name);
          }

          std::vector<std::string> unknown_species_solutes = FindUnknownSpecies(speices_in_solutes, species_registered_in_phase);
          if (!unknown_species_solutes.empty())
          {
            std::ostringstream oss;

            std::string line = std::to_string(object.Mark().line + 1);
            std::string column = std::to_string(object.Mark().column + 1);
            oss << line << ":" << column;
            oss << " error: Required '" << particle.phase << "' species ";

            for (size_t i = 0; i < unknown_species_solutes.size(); i++)
            {
              oss << "'" << unknown_species_solutes[i] << "'";
              if (i != unknown_species_solutes.size() - 1)
              {
                oss << ", ";
              }
            }

            if (object[validation::name])
            {
              oss << " in '" << object[validation::name].as<std::string>() << "' reaction";
            }
            else
            {
              oss << " in the reaction";
            }
            oss << " do not exist in the species registered in the '" << particle.phase << "' phase";

            errors.push_back({ ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase, oss.str() });
          }

          // Check whether the species in solvent belong to the corresponding phase
          bool found_solvent = std::find(species_registered_in_phase.begin(), species_registered_in_phase.end(), particle.solvent.species_name) !=
                               species_registered_in_phase.end();
          if (!found_solvent)
          {
            std::string line = std::to_string(object.Mark().line + 1);
            std::string column = std::to_string(object.Mark().column + 1);
            std::string message = line + ":" + column + ": Required " + particle.phase +
                                  " species as a solvent do not exist in the species registered in the " + particle.phase + " phase";
            errors.push_back({ ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase, message });
          }
        }

        henrys_law.gas = gas;
        henrys_law.particle = particle;
        henrys_law.unknown_properties = GetComments(object);
        reactions.henrys_law.push_back(henrys_law);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration
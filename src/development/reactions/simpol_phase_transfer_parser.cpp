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
    Errors SimpolPhaseTransferParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::SimpolPhaseTransfer simpol_phase_transfer;

      std::vector<std::string> required_keys = { validation::type,
                                                 validation::gas_phase,
                                                 validation::gas_phase_species,
                                                 validation::condensed_phase,
                                                 validation::condensed_phase_species,
                                                 validation::B };
      std::vector<std::string> optional_keys = { validation::name };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::string gas_phase_species = object[validation::gas_phase_species].as<std::string>();
        std::string condensed_phase_species = object[validation::condensed_phase_species].as<std::string>();

        if (object[validation::name])
        {
          simpol_phase_transfer.name = object[validation::name].as<std::string>();
        }

        std::vector<std::string> requested_species{ gas_phase_species, condensed_phase_species };
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

        std::string condensed_phase = object[validation::condensed_phase].as<std::string>();
        auto aqueous_it = std::find_if(
            existing_phases.begin(),
            existing_phases.end(),
            [&condensed_phase](const auto& phase) { return phase.name == condensed_phase; });
        if (aqueous_it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::condensed_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::condensed_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + condensed_phase });
        }
        else
        {
          auto phase = *aqueous_it;
          auto spec_it = std::find_if(
              phase.species.begin(),
              phase.species.end(),
              [&condensed_phase_species](const types::PhaseSpecies& species)
              { return species.name == condensed_phase_species; });
          if (spec_it == phase.species.end())
          {
            std::string line = std::to_string(object[validation::condensed_phase_species].Mark().line + 1);
            std::string column = std::to_string(object[validation::condensed_phase_species].Mark().column + 1);
            errors.push_back({ ConfigParseStatus::ReactionRequiresUnknownSpecies,
                               line + ":" + column + ": Unknown species: " + condensed_phase_species });
          }
        }

        std::string gas_phase = object[validation::gas_phase].as<std::string>();
        auto gas_it = std::find_if(
            existing_phases.begin(),
            existing_phases.end(),
            [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (gas_it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::gas_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::gas_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + gas_phase });
        }
        else
        {
          auto phase = *gas_it;
          auto spec_it = std::find_if(
              phase.species.begin(),
              phase.species.end(),
              [&gas_phase_species](const types::PhaseSpecies& species) { return species.name == gas_phase_species; });
          if (spec_it == phase.species.end())
          {
            std::string line = std::to_string(object[validation::gas_phase_species].Mark().line + 1);
            std::string column = std::to_string(object[validation::gas_phase_species].Mark().column + 1);
            errors.push_back({ ConfigParseStatus::ReactionRequiresUnknownSpecies,
                               line + ":" + column + ": Unknown species: " + gas_phase_species });
          }
        }

        if (object[validation::B] && object[validation::B].IsSequence() && object[validation::B].size() == 4)
        {
          for (size_t i = 0; i < 4; ++i)
          {
            simpol_phase_transfer.B[i] = object[validation::B][i].as<double>();
          }
        }

        simpol_phase_transfer.gas_phase = gas_phase;
        types::ReactionComponent gas_component;
        gas_component.species_name = gas_phase_species;
        simpol_phase_transfer.gas_phase_species = gas_component;
        simpol_phase_transfer.condensed_phase = condensed_phase;
        types::ReactionComponent aqueous_component;
        aqueous_component.species_name = condensed_phase_species;
        simpol_phase_transfer.condensed_phase_species = aqueous_component;
        simpol_phase_transfer.unknown_properties = GetComments(object);
        reactions.simpol_phase_transfer.push_back(simpol_phase_transfer);
      }

      return errors;
    }
  }  // namespace development
}  // namespace mechanism_configuration

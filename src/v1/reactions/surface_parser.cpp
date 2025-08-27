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
    Errors SurfaceParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::Surface surface;

      std::vector<std::string> required_keys = {
        validation::gas_phase_products, validation::gas_phase_species, validation::type, validation::gas_phase
      };
      std::vector<std::string> optional_keys = { validation::name, validation::reaction_probability };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::string gas_phase_species = object[validation::gas_phase_species].as<std::string>();

        auto products = ParseReactantsOrProducts(validation::gas_phase_products, object);
        errors.insert(errors.end(), products.first.begin(), products.first.end());

        if (object[validation::reaction_probability])
        {
          surface.reaction_probability = object[validation::reaction_probability].as<double>();
        }

        if (object[validation::name])
        {
          surface.name = object[validation::name].as<std::string>();
        }

        std::vector<std::string> requested_species;
        for (const auto& spec : products.second)
        {
          requested_species.push_back(spec.species_name);
        }
        requested_species.push_back(gas_phase_species);

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
            existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::gas_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::gas_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + gas_phase });
        }

        surface.gas_phase = gas_phase;
        surface.gas_phase_products = products.second;
        types::ReactionComponent component;
        component.species_name = gas_phase_species;
        surface.gas_phase_species = component;
        surface.unknown_properties = GetComments(object);
        reactions.surface.push_back(surface);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

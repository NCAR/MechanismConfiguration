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
    Errors TroeParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::Troe troe;

      std::vector<std::string> required_keys = {
        validation::products, validation::reactants, validation::type, validation::gas_phase
      };
      std::vector<std::string> optional_keys = { validation::name,   validation::k0_A,   validation::k0_B,
                                                 validation::k0_C,   validation::kinf_A, validation::kinf_B,
                                                 validation::kinf_C, validation::Fc,     validation::N };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        auto products = ParseReactantsOrProducts(validation::products, object);
        errors.insert(errors.end(), products.first.begin(), products.first.end());
        auto reactants = ParseReactantsOrProducts(validation::reactants, object);
        errors.insert(errors.end(), reactants.first.begin(), reactants.first.end());

        if (object[validation::k0_A])
        {
          troe.k0_A = object[validation::k0_A].as<double>();
        }
        if (object[validation::k0_B])
        {
          troe.k0_B = object[validation::k0_B].as<double>();
        }
        if (object[validation::k0_C])
        {
          troe.k0_C = object[validation::k0_C].as<double>();
        }
        if (object[validation::kinf_A])
        {
          troe.kinf_A = object[validation::kinf_A].as<double>();
        }
        if (object[validation::kinf_B])
        {
          troe.kinf_B = object[validation::kinf_B].as<double>();
        }
        if (object[validation::kinf_C])
        {
          troe.kinf_C = object[validation::kinf_C].as<double>();
        }
        if (object[validation::Fc])
        {
          troe.Fc = object[validation::Fc].as<double>();
        }
        if (object[validation::N])
        {
          troe.N = object[validation::N].as<double>();
        }

        if (object[validation::name])
        {
          troe.name = object[validation::name].as<std::string>();
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

        troe.gas_phase = gas_phase;
        troe.products = products.second;
        troe.reactants = reactants.second;
        troe.unknown_properties = GetComments(object);
        reactions.troe.push_back(troe);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

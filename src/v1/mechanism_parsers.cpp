// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/v1/mechanism_parsers.hpp>
#include <mechanism_configuration/v1/model_parsers.hpp>
#include <mechanism_configuration/v1/reaction_parsers.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/v1/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <sstream>

namespace mechanism_configuration
{
  namespace v1
  {
    std::pair<Errors, std::vector<v1::types::Species>> ParseSpecies(const YAML::Node& objects)

    {
      Errors errors;
      std::vector<types::Species> all_species;
      std::vector<std::pair<types::Species, YAML::Node>> species_node_pairs;

      for (const auto& object : objects)
      {
        types::Species species;
        std::vector<std::string> required_keys = { validation::name };
        std::vector<std::string> optional_keys = { validation::absolute_tolerance,
                                                   validation::diffusion_coefficient,
                                                   validation::molecular_weight,
                                                   validation::henrys_law_constant_298,
                                                   validation::henrys_law_constant_exponential_factor,
                                                   validation::n_star,
                                                   validation::density,
                                                   validation::tracer_type };
        auto validate = ValidateSchema(object, required_keys, optional_keys);
        errors.insert(errors.end(), validate.begin(), validate.end());
        if (validate.empty())
        {
          std::string name = object[validation::name].as<std::string>();
          species.name = name;

          if (object[validation::tracer_type])
            species.tracer_type = object[validation::tracer_type].as<std::string>();

          if (object[validation::absolute_tolerance])
            species.absolute_tolerance = object[validation::absolute_tolerance].as<double>();
          if (object[validation::diffusion_coefficient])
            species.diffusion_coefficient = object[validation::diffusion_coefficient].as<double>();
          if (object[validation::molecular_weight])
            species.molecular_weight = object[validation::molecular_weight].as<double>();
          if (object[validation::henrys_law_constant_298])
            species.henrys_law_constant_298 = object[validation::henrys_law_constant_298].as<double>();
          if (object[validation::henrys_law_constant_exponential_factor])
            species.henrys_law_constant_exponential_factor = object[validation::henrys_law_constant_exponential_factor].as<double>();
          if (object[validation::n_star])
            species.n_star = object[validation::n_star].as<double>();
          if (object[validation::density])
            species.density = object[validation::density].as<double>();

          species.unknown_properties = GetComments(object);

          all_species.push_back(species);
          species_node_pairs.push_back({ species, object });
        }
      }

      std::vector<DuplicateEntryInfo> duplicates = FindDuplicateObjectsByName<types::Species>(species_node_pairs);
      if (!duplicates.empty())
      {
        for (const auto& duplicate : duplicates)
        {
          size_t total = duplicate.nodes.size();

          for (size_t i = 0; i < total; ++i)
          {
            const auto& object = duplicate.nodes[i];
            std::string line = std::to_string(object.Mark().line + 1);
            std::string column = std::to_string(object.Mark().column + 1);

            std::ostringstream oss;
            oss << line << ":" << column << " error: Duplicate species name '" << duplicate.name << "' found (" << (i + 1) << " of " << total << ")";

            errors.push_back({ ConfigParseStatus::DuplicateSpeciesDetected, oss.str() });
          }
        }
      }

      return { errors, all_species };
    }

    std::pair<Errors, std::vector<types::Phase>> ParsePhases(const YAML::Node& objects, const std::vector<types::Species> existing_species)
    {
      Errors errors;
      ConfigParseStatus status = ConfigParseStatus::Success;
      std::vector<types::Phase> all_phases;
      std::vector<std::pair<types::Phase, YAML::Node>> phase_node_pairs;

      const std::vector<std::string> phase_required_keys = { validation::name, validation::species };
      const std::vector<std::string> phase_optional_keys = {};

      for (const auto& object : objects)
      {
        types::Phase phase;
        auto validate = ValidateSchema(object, phase_required_keys, phase_optional_keys);
        errors.insert(errors.end(), validate.begin(), validate.end());
        if (validate.empty())
        {
          std::string name = object[validation::name].as<std::string>();

          std::vector<std::string> species{};
          for (const auto& spec : object[validation::species])
          {
            species.push_back(spec.as<std::string>());
          }

          phase.name = name;
          phase.species = species;
          phase.unknown_properties = GetComments(object);

          // Check for duplicate species within this phase
          for (size_t i = 0; i < species.size(); ++i)
          {
            for (size_t j = i + 1; j < species.size(); ++j)
            {
              if (species[i] == species[j])
              {
                errors.push_back({ ConfigParseStatus::DuplicateSpeciesInPhaseDetected, 
                                  "Duplicate species '" + species[i] + "' found in phase '" + name + "'." });
              }
            }
          }

          std::vector<std::string> unknown_species = FindUnknownSpecies(species, existing_species);
          if (!unknown_species.empty())
          {
            std::ostringstream oss;
            oss << " error: Phase '" << phase.name << "' requires unknown species: ";
            for (size_t i = 0; i < unknown_species.size(); i++)
            {
              oss << "'" << unknown_species[i] << "'";
              if (i != unknown_species.size()-1)
              {
                oss << ", ";
              }
            }
            errors.push_back({ ConfigParseStatus::PhaseRequiresUnknownSpecies, oss.str() });
          }

          all_phases.push_back(phase);
          phase_node_pairs.push_back({ phase, object });
        }
      }

      std::vector<DuplicateEntryInfo> duplicates = FindDuplicateObjectsByName<types::Phase>(phase_node_pairs);
      if (!duplicates.empty())
      {
        for (const auto& duplicate : duplicates)
        {
          size_t total = duplicate.nodes.size();

          for (size_t i = 0; i < total; ++i)
          {
            const auto& object = duplicate.nodes[i];
            std::string line = std::to_string(object.Mark().line + 1);
            std::string column = std::to_string(object.Mark().column + 1);

            std::ostringstream oss;
            oss << line << ":" << column << " error: Duplicate phase name '" << duplicate.name << "' found (" << (i + 1) << " of " << total << ")";

            errors.push_back({ ConfigParseStatus::DuplicatePhasesDetected, oss.str() });
          }
        }
      }

      return { errors, all_phases };
    }

    std::pair<Errors, types::ReactionComponent> ParseReactionComponent(const YAML::Node& object)
    {
      Errors errors;
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::ReactionComponent component;
      const std::vector<std::string> reaction_component_required_keys = { validation::species_name };
      const std::vector<std::string> reaction_component_optional_keys = { validation::coefficient };

      auto validate = ValidateSchema(object, reaction_component_required_keys, reaction_component_optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
        if (status == ConfigParseStatus::Success)
        {
          std::string species_name = object[validation::species_name].as<std::string>();
          double coefficient = 1;
          if (object[validation::coefficient])
          {
            coefficient = object[validation::coefficient].as<double>();
          }

          component.species_name = species_name;
          component.coefficient = coefficient;
          component.unknown_properties = GetComments(object);
        }

      return { errors, component };
    }

    std::pair<Errors, std::vector<types::ReactionComponent>> ParseReactantsOrProducts(const std::string& key, const YAML::Node& object)
    {
      Errors errors;
      std::vector<types::ReactionComponent> result{};
      for (const auto& product : object[key])
      {
        auto component_parse = ParseReactionComponent(product);
        errors.insert(errors.end(), component_parse.first.begin(), component_parse.first.end());
        if (component_parse.first.empty())
        {
          result.push_back(component_parse.second);
        }
      }
      return { errors, result };
    }

    std::pair<Errors, types::Reactions>
    ParseReactions(const YAML::Node& objects, const std::vector<types::Species>& existing_species, const std::vector<types::Phase>& existing_phases)
    {
      Errors errors;
      types::Reactions reactions;

      std::map<std::string, std::unique_ptr<IReactionParser>> parsers;
      parsers[validation::Arrhenius_key] = std::make_unique<ArrheniusParser>();
      parsers[validation::HenrysLaw_key] = std::make_unique<HenrysLawParser>();
      parsers[validation::WetDeposition_key] = std::make_unique<WetDepositionParser>();
      parsers[validation::AqueousPhaseEquilibrium_key] = std::make_unique<AqueousEquilibriumParser>();
      parsers[validation::SimpolPhaseTransfer_key] = std::make_unique<SimpolPhaseTransferParser>();
      parsers[validation::FirstOrderLoss_key] = std::make_unique<FirstOrderLossParser>();
      parsers[validation::Emission_key] = std::make_unique<EmissionParser>();
      parsers[validation::CondensedPhasePhotolysis_key] = std::make_unique<CondensedPhasePhotolysisParser>();
      parsers[validation::Photolysis_key] = std::make_unique<PhotolysisParser>();
      parsers[validation::Surface_key] = std::make_unique<SurfaceParser>();
      parsers[validation::TaylorSeries_key] = std::make_unique<TaylorSeriesParser>();
      parsers[validation::Tunneling_key] = std::make_unique<TunnelingParser>();
      parsers[validation::Branched_key] = std::make_unique<BranchedParser>();
      parsers[validation::Troe_key] = std::make_unique<TroeParser>();
      parsers[validation::CondensedPhaseArrhenius_key] = std::make_unique<CondensedPhaseArrheniusParser>();
      parsers[validation::UserDefined_key] = std::make_unique<UserDefinedParser>();

      for (const auto& object : objects)
      {
        std::string type = object[validation::type].as<std::string>();
        auto it = parsers.find(type);
        if (it != parsers.end())
        {
          auto parse_errors = it->second->parse(object, existing_species, existing_phases, reactions);
          errors.insert(errors.end(), parse_errors.begin(), parse_errors.end());
        }
        else
        {
          std::string line = std::to_string(object[validation::type].Mark().line + 1);
          std::string column = std::to_string(object[validation::type].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownType, "Unknown type: " + type + " at line " + line + " column " + column });
        }
      }

      return { errors, reactions };
    }

    std::pair<Errors, types::Models> ParseModels(const YAML::Node& objects, const std::vector<types::Phase>& existing_phases)
    {
      Errors errors;
      types::Models models;

      std::map<std::string, std::unique_ptr<IModelParser>> parsers;
      parsers[validation::GasModel_key] = std::make_unique<GasModelParser>();
      parsers[validation::ModalModel_key] = std::make_unique<ModalModelParser>();

      for (const auto& object : objects)
      {
        std::string type = object[validation::type].as<std::string>();
        auto it = parsers.find(type);
        if (it != parsers.end())
        {
          auto parse_errors = it->second->parse(object, existing_phases, models);
          errors.insert(errors.end(), parse_errors.begin(), parse_errors.end());
        }
        else
        {
          std::string line = std::to_string(object[validation::type].Mark().line + 1);
          std::string column = std::to_string(object[validation::type].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownType, "Unknown type: " + type + " at line " + line + " column " + column });
        }
      }

      return { errors, models };
    }

  }  // namespace v1
}  // namespace mechanism_configuration

// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/mechanism_parsers.hpp>
#include <mechanism_configuration/development/model_parsers.hpp>
#include <mechanism_configuration/development/reaction_parsers.hpp>
#include <mechanism_configuration/development/utils.hpp>
#include <mechanism_configuration/development/validation.hpp>
#include <mechanism_configuration/development/validator.hpp>
#include <mechanism_configuration/validate_schema.hpp>
#include <mechanism_configuration/error_location.hpp>

#include <sstream>

namespace
{
  std::string FormatYamlError(const YAML::Node& node, const std::string& message)
  {
    std::string line = std::to_string(node.Mark().line + 1);
    std::string column = std::to_string(node.Mark().column + 1);
    std::ostringstream oss;
    oss << line << ":" << column << message;
    return oss.str();
  }
}  // namespace

namespace mechanism_configuration
{
  namespace development
  {
    std::vector<types::Species> ParseSpecies(const YAML::Node& objects)
    {
      std::vector<types::Species> all_species;
      for (const auto& object : objects)
      {
        types::Species species;

        species.name = object[validation::name].as<std::string>();

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
          species.henrys_law_constant_exponential_factor =
              object[validation::henrys_law_constant_exponential_factor].as<double>();
        if (object[validation::n_star])
          species.n_star = object[validation::n_star].as<double>();
        if (object[validation::density])
          species.density = object[validation::density].as<double>();
        if (object[validation::constant_concentration])
          species.constant_concentration = object[validation::constant_concentration].as<double>();
        if (object[validation::constant_mixing_ratio])
          species.constant_mixing_ratio = object[validation::constant_mixing_ratio].as<double>();
        if (object[validation::is_third_body])
          species.is_third_body = object[validation::is_third_body].as<bool>();

        species.unknown_properties = std::move(GetComments(object));

        all_species.push_back(species);
      }
      return all_species;
    }

    std::vector<types::Phase> ParsePhases(const YAML::Node& objects)
    {
      std::vector<types::Phase> all_phases;
      for (const auto& object : objects)
      {
        types::Phase phase;
        phase.name = object[validation::name].as<std::string>();

        std::vector<types::PhaseSpecies> species;

        for (const auto& spec : object[validation::species])
        {
          types::PhaseSpecies phase_species;

          phase_species.name = spec[validation::name].as<std::string>();
          if (spec[validation::diffusion_coefficient])
          {
            phase_species.diffusion_coefficient = spec[validation::diffusion_coefficient].as<double>();
          }
          phase_species.unknown_properties = std::move(GetComments(spec));

          species.emplace_back(phase_species);
        }

        phase.species = species;
        phase.unknown_properties = std::move(GetComments(object));
        all_phases.emplace_back(phase);
      }

      return all_phases;
    }

    std::vector<types::ReactionComponent> ParseReactionComponent(
      const YAML::Node& object, 
      const std::string& key)
    {
      std::vector<types::ReactionComponent> component_list;
      for (const auto& elem : AsSequence(object[key]))
      {
        types::ReactionComponent component;
        component.name = elem[validation::name].as<std::string>();
        component.unknown_properties = GetComments(elem);

        if (elem[validation::coefficient])
        {
          component.coefficient = elem[validation::coefficient].as<double>();
        }
        
        component_list.emplace_back(std::move(component));
      }

      return component_list;
    };

    types::Reactions ParseReactions(const YAML::Node& objects)
    {
      auto& parsers = GetReactionParserMap();
      types::Reactions reactions;

      for (const auto& object : objects)
      {
        auto it = parsers.find(object[validation::type].as<std::string>());
        if (it != parsers.end())
        {
          it->second->Parse(object, reactions);
        }
      }

      return reactions;
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
          errors.push_back(
              { ConfigParseStatus::UnknownType, "Unknown type: " + type + " at line " + line + " column " + column });
        }
      }

      return { errors, models };
    }

  }  // namespace development
}  // namespace mechanism_configuration

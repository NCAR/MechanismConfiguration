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

    std::pair<Errors, std::vector<types::ReactionComponent>> ParseReactantsOrProducts(
        const std::string& key,
        const YAML::Node& object)
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

    std::pair<Errors, types::Reactions> ParseReactions(
        const YAML::Node& objects,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      Errors errors;

      auto& parsers = GetReactionParserMap();
      types::Reactions reactions;

      for (const auto& object : objects)
      {
        std::string type = object[validation::type].as<std::string>();
        auto it = parsers.find(type);
        if (it != parsers.end())
        {
          // TODO - This is temporary until the decoupling is complete
          if (type == validation::Arrhenius_key || type == validation::Branched_key || type == validation::Emission_key ||
              type == validation::FirstOrderLoss_key || type == validation::Photolysis_key ||
              type == validation::Surface_key || type == validation::TaylorSeries_key || type == validation::Troe_key ||
              type == validation::Tunneling_key || type == validation::TernaryChemicalActivation_key ||
              type == validation::UserDefined_key)
          {
            it->second->Parse(object, reactions);
          }
          else
          {
            auto parse_errors = it->second->parse(object, existing_species, existing_phases, reactions);
            errors.insert(errors.end(), parse_errors.begin(), parse_errors.end());
          }
        }
        else
        {
          std::string line = std::to_string(object[validation::type].Mark().line + 1);
          std::string column = std::to_string(object[validation::type].Mark().column + 1);
          errors.push_back(
              { ConfigParseStatus::UnknownType, "Unknown type: " + type + " at line " + line + " column " + column });
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
          errors.push_back(
              { ConfigParseStatus::UnknownType, "Unknown type: " + type + " at line " + line + " column " + column });
        }
      }

      return { errors, models };
    }

  }  // namespace development
}  // namespace mechanism_configuration

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/type_parsers.hpp"

#include "detail/v1/keys.hpp"
#include "detail/v1/reaction_parsers.hpp"
#include "detail/v1/utils.hpp"

#include <mechanism_configuration/errors.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    std::vector<types::Species> ParseSpecies(const YAML::Node& objects)
    {
      std::vector<types::Species> all_species;
      for (const auto& object : objects)
      {
        types::Species species;

        species.name = object[keys::name].as<std::string>();

        if (object[keys::tracer_type])
          species.tracer_type = object[keys::tracer_type].as<std::string>();
        if (object[keys::absolute_tolerance])
          species.absolute_tolerance = object[keys::absolute_tolerance].as<double>();
        if (object[keys::diffusion_coefficient])
          species.diffusion_coefficient = object[keys::diffusion_coefficient].as<double>();
        if (object[keys::molecular_weight])
          species.molecular_weight = object[keys::molecular_weight].as<double>();
        if (object[keys::henrys_law_constant_298])
          species.henrys_law_constant_298 = object[keys::henrys_law_constant_298].as<double>();
        if (object[keys::henrys_law_constant_exponential_factor])
          species.henrys_law_constant_exponential_factor = object[keys::henrys_law_constant_exponential_factor].as<double>();
        if (object[keys::n_star])
          species.n_star = object[keys::n_star].as<double>();
        if (object[keys::density])
          species.density = object[keys::density].as<double>();
        if (object[keys::constant_concentration])
          species.constant_concentration = object[keys::constant_concentration].as<double>();
        if (object[keys::constant_mixing_ratio])
          species.constant_mixing_ratio = object[keys::constant_mixing_ratio].as<double>();
        if (object[keys::is_third_body])
          species.is_third_body = object[keys::is_third_body].as<bool>();

        species.unknown_properties = GetComments(object);

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
        phase.name = object[keys::name].as<std::string>();

        std::vector<types::PhaseSpecies> species;

        for (const auto& spec : object[keys::species])
        {
          types::PhaseSpecies phase_species;

          if (spec.IsScalar())
          {
            // Shorthand: a bare string is the species name.
            phase_species.name = spec.as<std::string>();
          }
          else
          {
            // Object form: a name plus optional properties.
            phase_species.name = spec[keys::name].as<std::string>();
            if (spec[keys::diffusion_coefficient])
            {
              phase_species.diffusion_coefficient = spec[keys::diffusion_coefficient].as<double>();
            }
            if (spec[keys::density])
            {
              phase_species.density = spec[keys::density].as<double>();
            }
            phase_species.unknown_properties = GetComments(spec);
          }

          species.emplace_back(phase_species);
        }

        phase.species = species;
        phase.unknown_properties = GetComments(object);
        all_phases.emplace_back(phase);
      }

      return all_phases;
    }

    std::vector<types::ReactionComponent> ParseReactionComponents(const YAML::Node& object, std::string_view key)
    {
      std::vector<types::ReactionComponent> component_list;
      for (const auto& elem : AsSequence(object[key]))
      {
        types::ReactionComponent component;
        component.name = GetReactionComponentName(elem);

        // A bare-string component carries only its name; coefficients/comments
        // are only present on the object form.
        if (!elem.IsScalar())
        {
          component.unknown_properties = GetComments(elem);
          if (elem[keys::coefficient])
          {
            component.coefficient = elem[keys::coefficient].as<double>();
          }
        }

        component_list.emplace_back(std::move(component));
      }

      return component_list;
    };

    types::ReactionComponent ParseReactionComponent(const YAML::Node& object, std::string_view key)
    {
      auto reaction_components = ParseReactionComponents(object, key);

      if (reaction_components.empty())
      {
        return types::ReactionComponent();
      }

      return std::move(reaction_components.front());
    };

    types::Reactions ParseReactions(const YAML::Node& objects)
    {
      auto& parsers = GetReactionParserMap();
      types::Reactions reactions;

      for (const auto& object : objects)
      {
        auto it = parsers.find(object[keys::type].as<std::string>());
        if (it != parsers.end())
        {
          it->second->Parse(object, reactions);
        }
      }

      return reactions;
    }

  }  // namespace v1
}  // namespace mechanism_configuration

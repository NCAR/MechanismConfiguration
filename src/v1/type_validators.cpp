// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/v1/reaction_parsers.hpp>
#include <mechanism_configuration/v1/type_validators.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validation.hpp>
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <string>
#include <vector>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors ValidateSpecies(const YAML::Node& species_list)
    {
      const std::vector<std::string_view> required_keys = { validation::name };
      const std::vector<std::string_view> optional_keys = { validation::absolute_tolerance,
                                                       validation::diffusion_coefficient,
                                                       validation::molecular_weight,
                                                       validation::henrys_law_constant_298,
                                                       validation::henrys_law_constant_exponential_factor,
                                                       validation::n_star,
                                                       validation::density,
                                                       validation::tracer_type,
                                                       validation::constant_concentration,
                                                       validation::constant_mixing_ratio,
                                                       validation::is_third_body };
      // Structural validation only. Duplicate-species detection (a semantic check) is performed
      // by the version-neutral ValidateSemantics.
      Errors errors;
      for (const auto& object : species_list)
      {
        auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
      }
      return errors;
    }

    Errors ValidatePhases(const YAML::Node& phases_list, const std::vector<types::Species>& existing_species)
    {
      // Phase
      const std::vector<std::string_view> required_keys = { validation::name, validation::species };
      const std::vector<std::string_view> optional_keys = {};
      // PhaseSpecies
      const std::vector<std::string_view> species_required_keys = { validation::name };
      const std::vector<std::string_view> species_optional_keys = { validation::diffusion_coefficient };

      // Structural validation only. Duplicate detection, phase-species existence, and
      // phase-membership (semantic checks) are performed by the version-neutral
      // ValidateSemantics. existing_species is intentionally unused here.
      (void)existing_species;
      Errors errors;
      for (const auto& object : AsSequence(phases_list))
      {
        auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
        errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());

        for (const auto& spec : object[validation::species])
        {
          // A bare string is shorthand for a species name and needs no schema validation.
          if (spec.IsScalar())
            continue;
          auto species_validation_errors = ValidateSchema(spec, species_required_keys, species_optional_keys);
          errors.insert(errors.end(), species_validation_errors.begin(), species_validation_errors.end());
        }
      }
      return errors;
    }

    Errors ValidateReactantsOrProducts(const YAML::Node& list)
    {
      const std::vector<std::string_view> required_keys = {};
      const std::vector<std::string_view> optional_keys = { validation::coefficient };
      // A component's species reference may use the canonical `name` or the legacy
      // `species name` alias, but exactly one of them.
      const std::vector<std::vector<std::string_view>> exactly_one_of = {
        { validation::name, validation::species_name }
      };

      Errors errors;

      for (const auto& object : list)
      {
        auto validation_errors = ValidateSchema(object, required_keys, optional_keys, exactly_one_of);
        if (!validation_errors.empty())
        {
          errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        }
      }

      return errors;
    }

    Errors ValidateParticles(const YAML::Node& list)
    {
      const std::vector<std::string_view> required_keys = { validation::phase, validation::solutes, validation::solvent };
      const std::vector<std::string_view> optional_keys = {};

      Errors errors;

      for (const auto& object : AsSequence(list))
      {
        auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
        if (!validation_errors.empty())
        {
          errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        }

        // Solutes
        validation_errors = ValidateReactantsOrProducts(object[validation::solutes]);
        if (!validation_errors.empty())
        {
          errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        }

        // Solvent
        validation_errors = ValidateReactantsOrProducts(object[validation::solvent]);
        if (!validation_errors.empty())
        {
          errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        }
      }

      return errors;
    }

    Errors ValidateReactions(
        const YAML::Node& reactions_list,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      Errors errors;
      bool is_valid = true;

      auto& parsers = GetReactionParserMap();

      std::vector<std::pair<YAML::Node, IReactionParser*>> valid_reactions;

      for (const auto& object : reactions_list)
      {
        if (!object[validation::type])
        {
          ErrorLocation error_location{ object.Mark().line, object.Mark().column };
          std::string message = mc_fmt::format("{} error: Missing 'type' object in reaction.", error_location);
          errors.push_back({ ErrorCode::RequiredKeyNotFound, message });
          is_valid = false;
          continue;
        }

        std::string type = object[validation::type].as<std::string>();

        auto it = parsers.find(type);
        if (it == parsers.end())
        {
          const auto& node = object[validation::type];
          ErrorLocation error_location{ node.Mark().line, node.Mark().column };

          std::string message = mc_fmt::format("{} error: Unknown reaction type '{}' found.", error_location, type);

          errors.push_back({ ErrorCode::UnknownType, message });
          is_valid = false;

          continue;
        }
        valid_reactions.emplace_back(object, it->second.get());
      }

      if (!is_valid)
        return errors;

      for (const auto& [reaction_node, parser] : valid_reactions)
      {
        auto validation_errors = parser->Validate(reaction_node, existing_species, existing_phases);
        if (!validation_errors.empty())
        {
          errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        }
      }

      return errors;
    }

  }  // namespace v1
}  // namespace mechanism_configuration
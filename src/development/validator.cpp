// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/error_location.hpp>
#include <mechanism_configuration/development/utils.hpp>
#include <mechanism_configuration/development/validation.hpp>
#include <mechanism_configuration/development/validator.hpp>
#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <format>
#include <string>
#include <vector>

namespace mechanism_configuration
{
  namespace development
  {

    Errors ValidateSpecies(const YAML::Node& species_list)
    {
      const std::vector<std::string> required_keys = { validation::name };
      const std::vector<std::string> optional_keys = { validation::absolute_tolerance,
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
      Errors errors;
      bool is_valid = true;

      std::vector<std::pair<types::Species, YAML::Node>> species_node_pairs;

      for (const auto& object : species_list)
      {
        auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
        if (!validation_errors.empty())
        {
          errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
          is_valid = false;
          continue;
        }

        // Get the name to check duplicate species
        types::Species species;
        species.name = object[validation::name].as<std::string>();

        species_node_pairs.emplace_back(species, object);
      }

      if (!is_valid)
        return errors;

      std::vector<DuplicateEntryInfo> duplicates = FindDuplicateObjectsByName<types::Species>(species_node_pairs);
      if (!duplicates.empty())
      {
        for (const auto& duplicate : duplicates)
        {
          size_t total = duplicate.nodes.size();

          for (size_t i = 0; i < total; ++i)
          {
            const auto& object = duplicate.nodes[i];
            ErrorLocation error_location{ object.Mark().line, object.Mark().column };

            std::string message = std::format("{} error: Duplicate species name '{}' found ({} of {})", error_location, duplicate.name, i + 1, total);

            errors.push_back({ ConfigParseStatus::DuplicateSpeciesDetected, message });
          }
        }
      }
      return errors;
    }

    Errors ValidatePhases(const YAML::Node& phases_list, const std::vector<types::Species>& existing_species)
    {
      // Phase
      const std::vector<std::string> required_keys = { validation::name, validation::species };
      const std::vector<std::string> optional_keys = {};
      // PhaseSpecies
      const std::vector<std::string> species_required_keys = { validation::name };
      const std::vector<std::string> species_optional_keys = { validation::diffusion_coefficient };

      Errors errors;
      bool is_valid = true;

      std::vector<std::pair<types::Phase, YAML::Node>> phase_node_pairs;

      for (const auto& object : phases_list)
      {
        auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
        if (!validation_errors.empty())
        {
          is_valid = false;
          errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
        }

        for (const auto& spec : object[validation::species])
        {
          auto species_validation_errors = ValidateSchema(spec, species_required_keys, species_optional_keys);
          if (!species_validation_errors.empty())
          {
            is_valid = false;
            errors.insert(errors.end(), species_validation_errors.begin(), species_validation_errors.end());
          }
        }

        if (!is_valid)
          return errors;

        types::Phase phase;
        phase.name = object[validation::name].as<std::string>();

        std::vector<std::pair<types::PhaseSpecies, YAML::Node>> species_node_pairs;

        for (const auto& spec : object[validation::species])
        {
          types::PhaseSpecies phase_species;
          phase_species.name = spec[validation::name].as<std::string>();

          if (spec[validation::diffusion_coefficient])
            phase_species.diffusion_coefficient = spec[validation::diffusion_coefficient].as<double>();

          species_node_pairs.emplace_back(phase_species, spec);
        }

        // Check for duplicate species within this phase
        std::vector<DuplicateEntryInfo> duplicates = FindDuplicateObjectsByName<types::PhaseSpecies>(species_node_pairs);
        if (!duplicates.empty())
        {
          for (const auto& duplicate : duplicates)
          {
            size_t total = duplicate.nodes.size();

            for (size_t i = 0; i < total; ++i)
            {
              const auto& duplicate_obj = duplicate.nodes[i];
              ErrorLocation error_location{ duplicate_obj.Mark().line, duplicate_obj.Mark().column };

              std::string message =
                  std::format("{} error: Duplicate species name '{}' found ({} of {})", error_location, duplicate.name, i + 1, total);

              errors.push_back({ ConfigParseStatus::DuplicateSpeciesInPhaseDetected, message });
            }
          }
        }

        // Check for unknown species within this phase
        std::vector<NodeInfo> unknown_species = FindUnknownObjectsByName(existing_species, species_node_pairs);
        if (!unknown_species.empty())
        {
          for (const auto& [name, node] : unknown_species)
          {
            ErrorLocation error_location{ node.Mark().line, node.Mark().column };

            std::string message = std::format("{} error: Unknown species name '{}' found in '{}' phase", error_location, name, phase.name);

            errors.push_back({ ConfigParseStatus::PhaseRequiresUnknownSpecies, message });
          }
        }
        phase_node_pairs.emplace_back(phase, object);
      }

      // Check for duplicate phase names
      std::vector<DuplicateEntryInfo> duplicates = FindDuplicateObjectsByName(phase_node_pairs);
      if (!duplicates.empty())
      {
        for (const auto& duplicate : duplicates)
        {
          size_t total = duplicate.nodes.size();

          for (size_t i = 0; i < total; ++i)
          {
            const auto& duplicate_object = duplicate.nodes[i];
            ErrorLocation error_location{ duplicate_object.Mark().line, duplicate_object.Mark().column };

            std::string message = std::format("{} error: Duplicate phase name '{}' found ({} of {})", error_location, duplicate.name, i + 1, total);

            errors.push_back({ ConfigParseStatus::DuplicatePhasesDetected, message });
          }
        }
      }
      return errors;
    }

  }  // namespace development
}  // namespace mechanism_configuration
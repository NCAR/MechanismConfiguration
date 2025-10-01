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
      std::vector<std::pair<types::Species, YAML::Node>> species_node_pairs;

      for (const auto& object : species_list)
      {
        auto validation_errors = ValidateSchema(object, required_keys, optional_keys);
        if (!validation_errors.empty())
        {
          errors.insert(errors.end(), validation_errors.begin(), validation_errors.end());
          continue;
        }

        // Get the name to check duplicate species
        types::Species species;
        species.name = object[validation::name].as<std::string>();

        species_node_pairs.emplace_back(species, object);
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
            ErrorLocation error_location{ object.Mark().line, object.Mark().column };

            std::string message = std::format("{} error: Duplicate species name '{}' found ({} of {})", error_location, duplicate.name, i + 1, total);

            errors.push_back({ ConfigParseStatus::DuplicateSpeciesDetected, message });
          }
        }
      }
      return errors;
    }

  }  // namespace development
}  // namespace mechanism_configuration
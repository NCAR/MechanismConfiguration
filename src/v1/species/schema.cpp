// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/species/schema.hpp"

#include "detail/error_format.hpp"
#include "detail/schema.hpp"
#include "detail/v1/keys.hpp"
#include "detail/v1/species/keys.hpp"
#include "detail/v1/utils.hpp"

#include <mechanism_configuration/errors.hpp>

#include <string>
#include <vector>

namespace mechanism_configuration::v1
{
  Errors CheckSpeciesSchema(const YAML::Node& species_list)
  {
    const std::vector<std::string_view> required_keys = { keys::name };
    const std::vector<std::string_view> optional_keys = { keys::absolute_tolerance,
                                                          keys::diffusion_coefficient,
                                                          keys::molecular_weight,
                                                          keys::henrys_law_constant_298,
                                                          keys::henrys_law_constant_exponential_factor,
                                                          keys::n_star,
                                                          keys::density,
                                                          keys::tracer_type,
                                                          keys::constant_concentration,
                                                          keys::constant_mixing_ratio,
                                                          keys::is_third_body };
    // Structural validation only. Duplicate-species detection (a semantic check) is performed
    // by the version-neutral ValidateReactionsSemantics.
    Errors errors;
    for (const auto& object : species_list)
    {
      auto schema_errors = CheckSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
    }
    return errors;
  }

  Errors CheckPhasesSchema(const YAML::Node& phases_list, const std::vector<types::Species>& existing_species)
  {
    // Phase
    const std::vector<std::string_view> required_keys = { keys::name, keys::species };
    const std::vector<std::string_view> optional_keys = {};
    // PhaseSpecies
    const std::vector<std::string_view> species_required_keys = { keys::name };
    const std::vector<std::string_view> species_optional_keys = { keys::diffusion_coefficient, keys::density };

    // Structural validation only. Duplicate detection, phase-species existence, and
    // phase-membership (semantic checks) are performed by the version-neutral
    // ValidateReactionsSemantics. existing_species is intentionally unused here.
    (void)existing_species;
    Errors errors;
    for (const auto& object : AsSequence(phases_list))
    {
      auto schema_errors = CheckSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());

      for (const auto& spec : object[keys::species])
      {
        // A bare string is shorthand for a species name and needs no schema validation.
        if (spec.IsScalar())
          continue;
        auto species_schema_errors = CheckSchema(spec, species_required_keys, species_optional_keys);
        errors.insert(errors.end(), species_schema_errors.begin(), species_schema_errors.end());
      }
    }
    return errors;
  }

}  // namespace mechanism_configuration::v1

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/aerosol/schema.hpp"

#include "detail/error_format.hpp"
#include "detail/schema.hpp"
#include "detail/v1/aerosol/keys.hpp"
#include "detail/v1/keys.hpp"
#include "detail/v1/reactions/schema.hpp"
#include "detail/v1/utils.hpp"

#include <mechanism_configuration/format_compat.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace mechanism_configuration::v1
{
  namespace
  {
    Errors CheckArrheniusReferenceTemperatureSchema(const YAML::Node& object)
    {
      const std::vector<std::string_view> required_keys = { keys::A, keys::henry_law_C };
      const std::vector<std::string_view> optional_keys = { keys::type, keys::reference_temperature };
      return CheckSchema(object, required_keys, optional_keys);
    }

    Errors CheckArrheniusSchema(const YAML::Node& object)
    {
      const std::vector<std::string_view> required_keys = { keys::A, keys::C };
      const std::vector<std::string_view> optional_keys = { keys::type };
      return CheckSchema(object, required_keys, optional_keys);
    }

    Errors CheckRateConstantSchema(const YAML::Node& object)
    {
      if (object[keys::type] && object[keys::type].as<std::string>() == keys::ArrheniusReferenceTemperature_key)
        return CheckArrheniusReferenceTemperatureSchema(object);
      return CheckArrheniusSchema(object);
    }

    Errors CheckHenryLawConstantSchema(const YAML::Node& object)
    {
      const std::vector<std::string_view> required_keys = { keys::HLC_ref, keys::henry_law_C };
      const std::vector<std::string_view> optional_keys = { keys::reference_temperature };
      return CheckSchema(object, required_keys, optional_keys);
    }

    // A per-representation rate-constant map. Each value is a typed rate-constant block.
    Errors CheckRateConstantMapSchema(const YAML::Node& object)
    {
      Errors errors;
      for (const auto& entry : object)
      {
        auto entry_errors = CheckRateConstantSchema(entry.second);
        errors.insert(errors.end(), entry_errors.begin(), entry_errors.end());
      }
      return errors;
    }

    // Each linear-constraint term references a species in a phase with a coefficient.
    Errors CheckLinearConstraintTermsSchema(const YAML::Node& object)
    {
      Errors errors;
      const std::vector<std::string_view> required_keys = { keys::phase, keys::name, keys::coefficient };
      const std::vector<std::string_view> optional_keys = {};
      for (const auto& term : object)
      {
        auto term_errors = CheckSchema(term, required_keys, optional_keys);
        errors.insert(errors.end(), term_errors.begin(), term_errors.end());
      }
      return errors;
    }
  }  // namespace

  Errors CheckAerosolRepresentationsSchema(const YAML::Node& representations_list)
  {
    Errors errors;

    for (const auto& object : AsSequence(representations_list))
    {
      // Every representation needs a type to dispatch the remaining required keys on.
      if (!object[keys::type])
      {
        ErrorLocation error_location{ object.Mark().line, object.Mark().column };
        errors.push_back({ ErrorCode::RequiredKeyNotFound,
                           mc_fmt::format("{} error: Missing 'type' object in aerosol representation.", error_location) });
        continue;
      }

      // Keys shared by every representation type.
      std::vector<std::string_view> required_keys = { keys::type, keys::name, keys::phases };
      std::vector<std::string_view> optional_keys = {};

      const std::string type = object[keys::type].as<std::string>();
      if (type == keys::UniformSection_key)
      {
        required_keys.push_back(keys::minimum_radius);
        required_keys.push_back(keys::maximum_radius);
      }
      else if (type == keys::SingleMomentMode_key)
      {
        required_keys.push_back(keys::geometric_mean_radius);
        required_keys.push_back(keys::geometric_standard_deviation);
      }
      else if (type == keys::TwoMomentMode_key)
      {
        required_keys.push_back(keys::geometric_standard_deviation);
      }
      else
      {
        const auto& node = object[keys::type];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };
        errors.push_back(
            { ErrorCode::UnknownType,
              mc_fmt::format("{} error: Unknown aerosol representation type '{}' found.", error_location, type) });
        continue;
      }

      auto schema_errors = CheckSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
    }

    return errors;
  }

  Errors CheckAerosolProcessesSchema(const YAML::Node& processes_list)
  {
    Errors errors;

    for (const auto& object : AsSequence(processes_list))
    {
      // Every entry needs a type to dispatch the remaining required keys on.
      if (!object[keys::type])
      {
        ErrorLocation error_location{ object.Mark().line, object.Mark().column };
        errors.push_back({ ErrorCode::RequiredKeyNotFound,
                           mc_fmt::format("{} error: Missing 'type' object in aerosol process.", error_location) });
        continue;
      }

      std::vector<std::string_view> required_keys;
      std::vector<std::string_view> optional_keys;
      Errors nested_errors;

      const std::string type = object[keys::type].as<std::string>();
      if (type == keys::HenryLawPhaseTransfer_key)
      {
        // The diffusion coefficient is not given here. It is sourced from the gas-phase
        // species' definition in the phases section (see ValidateAerosolModel).
        required_keys = { keys::type,
                          keys::gas_phase,
                          keys::gas_phase_species,
                          keys::condensed_phase,
                          keys::condensed_phase_species,
                          keys::solvent,
                          keys::henry_law_constant,
                          keys::accommodation_coefficient };
        if (object[keys::henry_law_constant])
          nested_errors = CheckHenryLawConstantSchema(object[keys::henry_law_constant]);
      }
      else if (type == keys::DissolvedReaction_key)
      {
        required_keys = { keys::type,      keys::condensed_phase, keys::solvent,
                          keys::reactants, keys::products,        keys::rate_constants };
        if (object[keys::reactants])
        {
          auto e = CheckReactantsOrProductsSchema(object[keys::reactants]);
          nested_errors.insert(nested_errors.end(), e.begin(), e.end());
        }
        if (object[keys::products])
        {
          auto e = CheckReactantsOrProductsSchema(object[keys::products]);
          nested_errors.insert(nested_errors.end(), e.begin(), e.end());
        }
        if (object[keys::rate_constants])
        {
          auto e = CheckRateConstantMapSchema(object[keys::rate_constants]);
          nested_errors.insert(nested_errors.end(), e.begin(), e.end());
        }
      }
      else if (type == keys::DissolvedReversibleReaction_key)
      {
        required_keys = { keys::type, keys::condensed_phase, keys::solvent, keys::reactants, keys::products };
        optional_keys = { keys::forward_rate_constants, keys::reverse_rate_constants, keys::equilibrium_constant };
        if (object[keys::reactants])
        {
          auto e = CheckReactantsOrProductsSchema(object[keys::reactants]);
          nested_errors.insert(nested_errors.end(), e.begin(), e.end());
        }
        if (object[keys::products])
        {
          auto e = CheckReactantsOrProductsSchema(object[keys::products]);
          nested_errors.insert(nested_errors.end(), e.begin(), e.end());
        }
        if (object[keys::forward_rate_constants])
        {
          auto e = CheckRateConstantMapSchema(object[keys::forward_rate_constants]);
          nested_errors.insert(nested_errors.end(), e.begin(), e.end());
        }
        if (object[keys::reverse_rate_constants])
        {
          auto e = CheckRateConstantMapSchema(object[keys::reverse_rate_constants]);
          nested_errors.insert(nested_errors.end(), e.begin(), e.end());
        }
        if (object[keys::equilibrium_constant])
        {
          auto e = CheckArrheniusReferenceTemperatureSchema(object[keys::equilibrium_constant]);
          nested_errors.insert(nested_errors.end(), e.begin(), e.end());
        }
      }
      else if (type == keys::HenryLawEquilibrium_key)
      {
        // The solvent's molecular weight and density are not given here; they are sourced from
        // the solvent species' definition (see ValidateAerosolModel).
        required_keys = { keys::type,
                          keys::gas_phase,
                          keys::gas_phase_species,
                          keys::condensed_phase,
                          keys::condensed_phase_species,
                          keys::solvent,
                          keys::henry_law_constant };
        if (object[keys::henry_law_constant])
          nested_errors = CheckHenryLawConstantSchema(object[keys::henry_law_constant]);
      }
      else if (type == keys::DissolvedEquilibrium_key)
      {
        required_keys = { keys::type,
                          keys::condensed_phase,
                          keys::solvent,
                          keys::reactants,
                          keys::products,
                          keys::algebraic_species,
                          keys::equilibrium_constant };
        if (object[keys::reactants])
        {
          auto e = CheckReactantsOrProductsSchema(object[keys::reactants]);
          nested_errors.insert(nested_errors.end(), e.begin(), e.end());
        }
        if (object[keys::products])
        {
          auto e = CheckReactantsOrProductsSchema(object[keys::products]);
          nested_errors.insert(nested_errors.end(), e.begin(), e.end());
        }
        if (object[keys::equilibrium_constant])
        {
          auto e = CheckArrheniusReferenceTemperatureSchema(object[keys::equilibrium_constant]);
          nested_errors.insert(nested_errors.end(), e.begin(), e.end());
        }
      }
      else if (type == keys::LinearConstraint_key)
      {
        required_keys = { keys::type, keys::algebraic_phase, keys::algebraic_species, keys::terms };
        optional_keys = { keys::constant, keys::diagnose_from_state };
        if (object[keys::terms])
        {
          auto e = CheckLinearConstraintTermsSchema(object[keys::terms]);
          nested_errors.insert(nested_errors.end(), e.begin(), e.end());
        }
        // The RHS constant is given either as a fixed value or diagnosed from state, not both.
        if (object[keys::constant] && object[keys::diagnose_from_state])
        {
          const auto& node = object[keys::diagnose_from_state];
          ErrorLocation error_location{ node.Mark().line, node.Mark().column };
          nested_errors.push_back({ ErrorCode::MutuallyExclusiveOption,
                                    mc_fmt::format(
                                        "{} error: Mutually exclusive option of '{}' and '{}' found in '{}'.",
                                        error_location,
                                        keys::constant,
                                        keys::diagnose_from_state,
                                        type) });
        }
      }
      else
      {
        const auto& node = object[keys::type];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };
        errors.push_back({ ErrorCode::UnknownType,
                           mc_fmt::format("{} error: Unknown aerosol process type '{}' found.", error_location, type) });
        continue;
      }

      auto schema_errors = CheckSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      errors.insert(errors.end(), nested_errors.begin(), nested_errors.end());
    }

    return errors;
  }

}  // namespace mechanism_configuration::v1

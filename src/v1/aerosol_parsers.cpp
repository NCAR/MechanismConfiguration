// Copyright (C) 2026 University Corporation for Atmospheric Research
//                     University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/v1/aerosol_parsers.hpp>
#include <mechanism_configuration/v1/mechanism_parsers.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/v1/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

#include <sstream>

namespace
{
  std::string FormatError(const YAML::Node& node, const std::string& message)
  {
    std::ostringstream oss;
    oss << (node.Mark().line + 1) << ":" << (node.Mark().column + 1) << message;
    return oss.str();
  }

  /// @brief Find a phase by name in existing_phases, return nullptr if not found
  const mechanism_configuration::v1::types::Phase* FindPhase(
      const std::string& name,
      const std::vector<mechanism_configuration::v1::types::Phase>& existing_phases)
  {
    for (const auto& p : existing_phases)
      if (p.name == name)
        return &p;
    return nullptr;
  }

  /// @brief Check that a species exists in a given phase
  bool SpeciesInPhase(
      const std::string& species_name,
      const mechanism_configuration::v1::types::Phase& phase)
  {
    for (const auto& s : phase.species)
      if (s.name == species_name)
        return true;
    return false;
  }
}  // namespace

namespace mechanism_configuration
{
  namespace v1
  {
    using namespace types;

    // ── Aerosol Representations ─────────────────────────────────────────

    std::pair<Errors, std::vector<AerosolRepresentation>> ParseAerosolRepresentations(
        const YAML::Node& objects,
        const std::vector<Phase>& existing_phases)
    {
      Errors errors;
      std::vector<AerosolRepresentation> representations;

      for (const auto& object : objects)
      {
        if (!object[validation::type])
        {
          errors.push_back(
              { ConfigParseStatus::RequiredKeyNotFound,
                FormatError(object, " error: Aerosol representation missing 'type'") });
          continue;
        }

        std::string type_str = object[validation::type].as<std::string>();

        if (type_str == validation::UniformSection_key)
        {
          std::vector<std::string> required = { validation::type, validation::name, validation::phases,
                                                validation::minimum_radius, validation::maximum_radius };
          auto validate = ValidateSchema(object, required, {});
          errors.insert(errors.end(), validate.begin(), validate.end());
          if (!validate.empty())
            continue;

          UniformSection rep;
          rep.name = object[validation::name].as<std::string>();
          rep.min_radius = object[validation::minimum_radius].as<double>();
          rep.max_radius = object[validation::maximum_radius].as<double>();
          rep.unknown_properties = GetComments(object);

          for (const auto& ph : object[validation::phases])
          {
            std::string phase_name = ph.as<std::string>();
            rep.phases.push_back(phase_name);
            if (!FindPhase(phase_name, existing_phases))
            {
              errors.push_back(
                  { ConfigParseStatus::UnknownPhase,
                    FormatError(object, " error: Representation '" + rep.name +
                                            "' references unknown phase '" + phase_name + "'") });
            }
          }

          representations.push_back(rep);
        }
        else if (type_str == validation::SingleMomentMode_key)
        {
          std::vector<std::string> required = { validation::type, validation::name, validation::phases,
                                                validation::geometric_mean_radius,
                                                validation::geometric_standard_deviation };
          auto validate = ValidateSchema(object, required, {});
          errors.insert(errors.end(), validate.begin(), validate.end());
          if (!validate.empty())
            continue;

          SingleMomentMode rep;
          rep.name = object[validation::name].as<std::string>();
          rep.geometric_mean_radius = object[validation::geometric_mean_radius].as<double>();
          rep.geometric_standard_deviation = object[validation::geometric_standard_deviation].as<double>();
          rep.unknown_properties = GetComments(object);

          for (const auto& ph : object[validation::phases])
          {
            std::string phase_name = ph.as<std::string>();
            rep.phases.push_back(phase_name);
            if (!FindPhase(phase_name, existing_phases))
            {
              errors.push_back(
                  { ConfigParseStatus::UnknownPhase,
                    FormatError(object, " error: Representation '" + rep.name +
                                            "' references unknown phase '" + phase_name + "'") });
            }
          }

          representations.push_back(rep);
        }
        else if (type_str == validation::TwoMomentMode_key)
        {
          std::vector<std::string> required = { validation::type, validation::name, validation::phases,
                                                validation::geometric_standard_deviation };
          auto validate = ValidateSchema(object, required, {});
          errors.insert(errors.end(), validate.begin(), validate.end());
          if (!validate.empty())
            continue;

          TwoMomentMode rep;
          rep.name = object[validation::name].as<std::string>();
          rep.geometric_standard_deviation = object[validation::geometric_standard_deviation].as<double>();
          rep.unknown_properties = GetComments(object);

          for (const auto& ph : object[validation::phases])
          {
            std::string phase_name = ph.as<std::string>();
            rep.phases.push_back(phase_name);
            if (!FindPhase(phase_name, existing_phases))
            {
              errors.push_back(
                  { ConfigParseStatus::UnknownPhase,
                    FormatError(object, " error: Representation '" + rep.name +
                                            "' references unknown phase '" + phase_name + "'") });
            }
          }

          representations.push_back(rep);
        }
        else
        {
          errors.push_back(
              { ConfigParseStatus::UnknownType,
                FormatError(object, " error: Unknown aerosol representation type '" + type_str + "'") });
        }
      }

      return { errors, representations };
    }

    // ── Helpers for aerosol process parsing ──────────────────────────────

    namespace
    {
      HenrysLawConstant ParseHLC(const YAML::Node& node)
      {
        HenrysLawConstant hlc;
        hlc.hlc_ref = node[validation::hlc_ref].as<double>();
        if (node[validation::C])
          hlc.C = node[validation::C].as<double>();
        return hlc;
      }

      AerosolArrheniusRate ParseAerosolArrhenius(const YAML::Node& node)
      {
        AerosolArrheniusRate rate;
        if (node[validation::A])
          rate.A = node[validation::A].as<double>();
        if (node[validation::C])
          rate.C = node[validation::C].as<double>();
        return rate;
      }

      AerosolEquilibriumConstant ParseEquilibriumConstant(const YAML::Node& node)
      {
        AerosolEquilibriumConstant eq;
        if (node[validation::A])
          eq.A = node[validation::A].as<double>();
        if (node[validation::C])
          eq.C = node[validation::C].as<double>();
        return eq;
      }

      /// @brief Validate that a species name exists in given phase
      void ValidateSpeciesInPhase(
          Errors& errors,
          const YAML::Node& node,
          const std::string& species_name,
          const std::string& phase_name,
          const std::vector<Phase>& existing_phases)
      {
        const Phase* phase = FindPhase(phase_name, existing_phases);
        if (!phase)
        {
          errors.push_back(
              { ConfigParseStatus::UnknownPhase,
                FormatError(node, " error: Unknown phase '" + phase_name + "'") });
          return;
        }
        if (!SpeciesInPhase(species_name, *phase))
        {
          errors.push_back(
              { ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase,
                FormatError(node, " error: Species '" + species_name +
                                      "' not found in phase '" + phase_name + "'") });
        }
      }
    }  // namespace

    // ── Aerosol Processes ───────────────────────────────────────────────

    std::pair<Errors, std::vector<AerosolProcess>> ParseAerosolProcesses(
        const YAML::Node& objects,
        const std::vector<Species>& existing_species,
        const std::vector<Phase>& existing_phases)
    {
      Errors errors;
      std::vector<AerosolProcess> processes;

      for (const auto& object : objects)
      {
        if (!object[validation::type])
        {
          errors.push_back(
              { ConfigParseStatus::RequiredKeyNotFound,
                FormatError(object, " error: Aerosol process missing 'type'") });
          continue;
        }

        std::string type_str = object[validation::type].as<std::string>();

        // ─── HENRY_LAW_EQUILIBRIUM ──────────────────────────────────────
        if (type_str == validation::HenryLawEquilibrium_key)
        {
          std::vector<std::string> required = {
            validation::type, validation::gas_phase, validation::gas_phase_species,
            validation::condensed_phase, validation::condensed_phase_species,
            validation::solvent, validation::henrys_law_constant
          };
          std::vector<std::string> optional = { validation::name };
          auto validate = ValidateSchema(object, required, optional);
          errors.insert(errors.end(), validate.begin(), validate.end());
          if (!validate.empty())
            continue;

          HenryLawEquilibrium proc;
          proc.gas_phase = object[validation::gas_phase].as<std::string>();
          proc.gas_phase_species = object[validation::gas_phase_species].as<std::string>();
          proc.condensed_phase = object[validation::condensed_phase].as<std::string>();
          proc.condensed_phase_species = object[validation::condensed_phase_species].as<std::string>();
          proc.solvent = object[validation::solvent].as<std::string>();
          proc.henrys_law_constant = ParseHLC(object[validation::henrys_law_constant]);
          if (object[validation::name])
            proc.name = object[validation::name].as<std::string>();
          proc.unknown_properties = GetComments(object);

          ValidateSpeciesInPhase(errors, object, proc.gas_phase_species, proc.gas_phase, existing_phases);
          ValidateSpeciesInPhase(errors, object, proc.condensed_phase_species, proc.condensed_phase, existing_phases);
          ValidateSpeciesInPhase(errors, object, proc.solvent, proc.condensed_phase, existing_phases);

          processes.push_back(proc);
        }
        // ─── HENRY_LAW_PHASE_TRANSFER ───────────────────────────────────
        else if (type_str == validation::HenryLawPhaseTransfer_key)
        {
          std::vector<std::string> required = {
            validation::type, validation::gas_phase, validation::gas_phase_species,
            validation::condensed_phase, validation::condensed_phase_species,
            validation::solvent, validation::henrys_law_constant,
            validation::diffusion_coefficient, validation::aerosol_accommodation_coefficient
          };
          std::vector<std::string> optional = { validation::name };
          auto validate = ValidateSchema(object, required, optional);
          errors.insert(errors.end(), validate.begin(), validate.end());
          if (!validate.empty())
            continue;

          HenryLawPhaseTransfer proc;
          proc.gas_phase = object[validation::gas_phase].as<std::string>();
          proc.gas_phase_species = object[validation::gas_phase_species].as<std::string>();
          proc.condensed_phase = object[validation::condensed_phase].as<std::string>();
          proc.condensed_phase_species = object[validation::condensed_phase_species].as<std::string>();
          proc.solvent = object[validation::solvent].as<std::string>();
          proc.henrys_law_constant = ParseHLC(object[validation::henrys_law_constant]);
          proc.diffusion_coefficient = object[validation::diffusion_coefficient].as<double>();
          proc.accommodation_coefficient = object[validation::aerosol_accommodation_coefficient].as<double>();
          if (object[validation::name])
            proc.name = object[validation::name].as<std::string>();
          proc.unknown_properties = GetComments(object);

          ValidateSpeciesInPhase(errors, object, proc.gas_phase_species, proc.gas_phase, existing_phases);
          ValidateSpeciesInPhase(errors, object, proc.condensed_phase_species, proc.condensed_phase, existing_phases);
          ValidateSpeciesInPhase(errors, object, proc.solvent, proc.condensed_phase, existing_phases);

          processes.push_back(proc);
        }
        // ─── DISSOLVED_REACTION ─────────────────────────────────────────
        else if (type_str == validation::DissolvedReaction_key)
        {
          std::vector<std::string> required = {
            validation::type, validation::condensed_phase, validation::solvent,
            validation::reactants, validation::products, validation::rate_constant
          };
          std::vector<std::string> optional = { validation::name, validation::min_halflife };
          auto validate = ValidateSchema(object, required, optional);
          errors.insert(errors.end(), validate.begin(), validate.end());
          if (!validate.empty())
            continue;

          DissolvedReaction proc;
          proc.condensed_phase = object[validation::condensed_phase].as<std::string>();
          proc.solvent = object[validation::solvent].as<std::string>();
          if (object[validation::name])
            proc.name = object[validation::name].as<std::string>();
          if (object[validation::min_halflife])
            proc.min_halflife = object[validation::min_halflife].as<double>();
          proc.unknown_properties = GetComments(object);

          // Parse rate constant (inline Arrhenius)
          auto rc_node = object[validation::rate_constant];
          proc.rate_constant = ParseAerosolArrhenius(rc_node);

          // Parse reactants and products
          auto [reactant_errors, reactants] = ParseReactantsOrProducts(validation::reactants, object);
          errors.insert(errors.end(), reactant_errors.begin(), reactant_errors.end());
          proc.reactants = reactants;

          auto [product_errors, products] = ParseReactantsOrProducts(validation::products, object);
          errors.insert(errors.end(), product_errors.begin(), product_errors.end());
          proc.products = products;

          // Validate species in phase
          const Phase* phase = FindPhase(proc.condensed_phase, existing_phases);
          if (phase)
          {
            for (const auto& r : proc.reactants)
              if (!SpeciesInPhase(r.species_name, *phase))
                errors.push_back(
                    { ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase,
                      FormatError(object, " error: Reactant '" + r.species_name +
                                              "' not in phase '" + proc.condensed_phase + "'") });
            for (const auto& p : proc.products)
              if (!SpeciesInPhase(p.species_name, *phase))
                errors.push_back(
                    { ConfigParseStatus::RequestedSpeciesNotRegisteredInPhase,
                      FormatError(object, " error: Product '" + p.species_name +
                                              "' not in phase '" + proc.condensed_phase + "'") });
          }
          else
          {
            errors.push_back(
                { ConfigParseStatus::UnknownPhase,
                  FormatError(object, " error: Unknown phase '" + proc.condensed_phase + "'") });
          }

          processes.push_back(proc);
        }
        // ─── DISSOLVED_REVERSIBLE_REACTION ──────────────────────────────
        else if (type_str == validation::DissolvedReversibleReaction_key)
        {
          std::vector<std::string> required = {
            validation::type, validation::condensed_phase, validation::solvent,
            validation::reactants, validation::products
          };
          std::vector<std::string> optional = {
            validation::name, validation::forward_rate_constant,
            validation::reverse_rate_constant, validation::equilibrium_constant
          };
          auto validate = ValidateSchema(object, required, optional);
          errors.insert(errors.end(), validate.begin(), validate.end());
          if (!validate.empty())
            continue;

          DissolvedReversibleReaction proc;
          proc.condensed_phase = object[validation::condensed_phase].as<std::string>();
          proc.solvent = object[validation::solvent].as<std::string>();
          if (object[validation::name])
            proc.name = object[validation::name].as<std::string>();
          proc.unknown_properties = GetComments(object);

          if (object[validation::forward_rate_constant])
            proc.forward_rate_constant = ParseAerosolArrhenius(object[validation::forward_rate_constant]);
          if (object[validation::reverse_rate_constant])
            proc.reverse_rate_constant = ParseAerosolArrhenius(object[validation::reverse_rate_constant]);
          if (object[validation::equilibrium_constant])
            proc.equilibrium_constant = ParseEquilibriumConstant(object[validation::equilibrium_constant]);

          // Validate: exactly 2 of 3 rate parameters
          int count = (proc.forward_rate_constant.has_value() ? 1 : 0) +
                      (proc.reverse_rate_constant.has_value() ? 1 : 0) +
                      (proc.equilibrium_constant.has_value() ? 1 : 0);
          if (count != 2)
          {
            errors.push_back(
                { ConfigParseStatus::InvalidParameterNumber,
                  FormatError(object, " error: DISSOLVED_REVERSIBLE_REACTION requires exactly 2 of: "
                                      "forward rate constant, reverse rate constant, equilibrium constant") });
          }

          auto [reactant_errors, reactants] = ParseReactantsOrProducts(validation::reactants, object);
          errors.insert(errors.end(), reactant_errors.begin(), reactant_errors.end());
          proc.reactants = reactants;

          auto [product_errors, products] = ParseReactantsOrProducts(validation::products, object);
          errors.insert(errors.end(), product_errors.begin(), product_errors.end());
          proc.products = products;

          processes.push_back(proc);
        }
        // ─── DISSOLVED_EQUILIBRIUM ──────────────────────────────────────
        else if (type_str == validation::DissolvedEquilibrium_key)
        {
          std::vector<std::string> required = {
            validation::type, validation::condensed_phase, validation::solvent,
            validation::reactants, validation::products,
            validation::algebraic_species, validation::equilibrium_constant
          };
          std::vector<std::string> optional = { validation::name };
          auto validate = ValidateSchema(object, required, optional);
          errors.insert(errors.end(), validate.begin(), validate.end());
          if (!validate.empty())
            continue;

          DissolvedEquilibrium proc;
          proc.condensed_phase = object[validation::condensed_phase].as<std::string>();
          proc.solvent = object[validation::solvent].as<std::string>();
          proc.algebraic_species = object[validation::algebraic_species].as<std::string>();
          proc.equilibrium_constant = ParseEquilibriumConstant(object[validation::equilibrium_constant]);
          if (object[validation::name])
            proc.name = object[validation::name].as<std::string>();
          proc.unknown_properties = GetComments(object);

          auto [reactant_errors, reactants] = ParseReactantsOrProducts(validation::reactants, object);
          errors.insert(errors.end(), reactant_errors.begin(), reactant_errors.end());
          proc.reactants = reactants;

          auto [product_errors, products] = ParseReactantsOrProducts(validation::products, object);
          errors.insert(errors.end(), product_errors.begin(), product_errors.end());
          proc.products = products;

          processes.push_back(proc);
        }
        // ─── LINEAR_CONSTRAINT ──────────────────────────────────────────
        else if (type_str == validation::LinearConstraint_key)
        {
          std::vector<std::string> required = {
            validation::type, validation::algebraic_phase,
            validation::algebraic_species, validation::terms
          };
          std::vector<std::string> optional = {
            validation::name, validation::constant_mol_m3, validation::diagnose_from_state
          };
          auto validate = ValidateSchema(object, required, optional);
          errors.insert(errors.end(), validate.begin(), validate.end());
          if (!validate.empty())
            continue;

          LinearConstraint proc;
          proc.algebraic_phase = object[validation::algebraic_phase].as<std::string>();
          proc.algebraic_species = object[validation::algebraic_species].as<std::string>();
          if (object[validation::name])
            proc.name = object[validation::name].as<std::string>();
          if (object[validation::constant_mol_m3])
            proc.constant = object[validation::constant_mol_m3].as<double>();
          if (object[validation::diagnose_from_state])
            proc.diagnose_from_state = object[validation::diagnose_from_state].as<bool>();
          proc.unknown_properties = GetComments(object);

          // Validate: must have either diagnose_from_state or constant
          if (!proc.diagnose_from_state && !object[validation::constant_mol_m3])
          {
            errors.push_back(
                { ConfigParseStatus::RequiredKeyNotFound,
                  FormatError(object, " error: LINEAR_CONSTRAINT requires either 'diagnose from state: true' "
                                      "or 'constant [mol m-3]'") });
          }

          // Validate algebraic species is in algebraic phase
          ValidateSpeciesInPhase(errors, object, proc.algebraic_species, proc.algebraic_phase, existing_phases);

          // Parse terms
          for (const auto& term_node : object[validation::terms])
          {
            if (!term_node[validation::phase_key] || !term_node[validation::species] ||
                !term_node[validation::coefficient])
            {
              errors.push_back(
                  { ConfigParseStatus::RequiredKeyNotFound,
                    FormatError(term_node, " error: LINEAR_CONSTRAINT term requires 'phase', 'species', 'coefficient'") });
              continue;
            }

            LinearConstraintTerm term;
            term.phase = term_node[validation::phase_key].as<std::string>();
            term.species = term_node[validation::species].as<std::string>();
            term.coefficient = term_node[validation::coefficient].as<double>();

            ValidateSpeciesInPhase(errors, term_node, term.species, term.phase, existing_phases);

            proc.terms.push_back(term);
          }

          processes.push_back(proc);
        }
        else
        {
          errors.push_back(
              { ConfigParseStatus::UnknownType,
                FormatError(object, " error: Unknown aerosol process type '" + type_str + "'") });
        }
      }

      return { errors, processes };
    }

  }  // namespace v1
}  // namespace mechanism_configuration

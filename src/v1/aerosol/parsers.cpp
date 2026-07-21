// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/aerosol/parsers.hpp"

#include "detail/v1/aerosol/keys.hpp"
#include "detail/v1/aerosol/utils.hpp"
#include "detail/v1/keys.hpp"
#include "detail/v1/reactions/parsers.hpp"

namespace mechanism_configuration::v1
{
  // ----------------------------------------
  // Rate constants parser
  // ----------------------------------------

  types::Equilibrium ParseEquilibrium(const YAML::Node& object)
  {
    types::Equilibrium rate_constant;

    rate_constant.A = object[keys::A].as<double>();
    if (object[keys::henrys_law_C])
      rate_constant.C = object[keys::henrys_law_C].as<double>();
    if (object[keys::reference_temperature])
      rate_constant.T0 = object[keys::reference_temperature].as<double>();

    return rate_constant;
  }

  types::Arrhenius ParseArrhenius(const YAML::Node& object)
  {
    types::Arrhenius rate_constant;

    rate_constant.A = object[keys::A].as<double>();
    if (object[keys::B])
      rate_constant.B = object[keys::B].as<double>();
    if (object[keys::C])
      rate_constant.C = object[keys::C].as<double>();
    if (object[keys::D])
      rate_constant.D = object[keys::D].as<double>();
    if (object[keys::E])
      rate_constant.E = object[keys::E].as<double>();

    return rate_constant;
  }

  types::RateConstant ParseRateConstant(const YAML::Node& object)
  {
    if (object[keys::type] && object[keys::type].as<std::string>() == keys::Equilibrium_key)
      return ParseEquilibrium(object);

    // ARRHENIUS is the default when no (recognized) type is given.
    return ParseArrhenius(object);
  }

  types::HenrysLawConstant ParseHenrysLawConstant(const YAML::Node& object)
  {
    types::HenrysLawConstant henrys_law_constant;

    henrys_law_constant.HLC_ref = object[keys::HLC_ref].as<double>();
    if (object[keys::henrys_law_C])
      henrys_law_constant.C = object[keys::henrys_law_C].as<double>();
    if (object[keys::reference_temperature])
      henrys_law_constant.T0 = object[keys::reference_temperature].as<double>();

    return henrys_law_constant;
  }

  // ----------------------------------------
  // Representation parsers
  // ----------------------------------------

  namespace
  {
    std::vector<std::string> ParsePhaseNames(const YAML::Node& object)
    {
      std::vector<std::string> phases;
      for (const auto& phase : object[keys::phases])
        phases.push_back(phase.as<std::string>());
      return phases;
    }
  }  // namespace

  types::UniformSection ParseUniformSection(const YAML::Node& object)
  {
    types::UniformSection uniform_section;

    uniform_section.name = object[keys::name].as<std::string>();
    uniform_section.phases = ParsePhaseNames(object);
    uniform_section.min_radius = object[keys::minimum_radius].as<double>();
    uniform_section.max_radius = object[keys::maximum_radius].as<double>();

    return uniform_section;
  }

  types::SingleMomentMode ParseSingleMomentMode(const YAML::Node& object)
  {
    types::SingleMomentMode single_moment_mode;

    single_moment_mode.name = object[keys::name].as<std::string>();
    single_moment_mode.phases = ParsePhaseNames(object);
    single_moment_mode.geometric_mean_radius = object[keys::geometric_mean_radius].as<double>();
    single_moment_mode.geometric_standard_deviation = object[keys::geometric_standard_deviation].as<double>();

    return single_moment_mode;
  }

  types::TwoMomentMode ParseTwoMomentMode(const YAML::Node& object)
  {
    types::TwoMomentMode two_moment_mode;

    two_moment_mode.name = object[keys::name].as<std::string>();
    two_moment_mode.phases = ParsePhaseNames(object);
    two_moment_mode.geometric_standard_deviation = object[keys::geometric_standard_deviation].as<double>();

    return two_moment_mode;
  }

  // ----------------------------------------
  // Process parsers
  // ----------------------------------------

  types::HenrysLawPhaseTransfer ParseHenrysLawPhaseTransfer(
      const YAML::Node& object,
      const std::vector<types::Phase>& phases)
  {
    types::HenrysLawPhaseTransfer transfer;

    transfer.gas_phase = object[keys::gas_phase].as<std::string>();
    transfer.gas_species = object[keys::gas_phase_species].as<std::string>();
    transfer.condensed_phase = object[keys::condensed_phase].as<std::string>();
    transfer.condensed_species = object[keys::condensed_phase_species].as<std::string>();
    transfer.solvent = object[keys::solvent].as<std::string>();
    transfer.henrys_law_constant = ParseHenrysLawConstant(object[keys::henrys_law_constant]);
    // Sourced from the gas-phase species' definition; presence is enforced by
    // ValidateAerosolSemantics, which runs before this parser is ever reached (in
    // Parser::ValidateAndBuild), so a missing value defaults harmlessly here.
    transfer.diffusion_coefficient =
        FindPhaseSpeciesDiffusionCoefficient(phases, transfer.gas_phase, transfer.gas_species).value_or(0.0);
    transfer.accommodation_coefficient = object[keys::accommodation_coefficient].as<double>();

    return transfer;
  }

  types::DissolvedReaction ParseDissolvedReaction(const YAML::Node& object)
  {
    types::DissolvedReaction reaction;

    reaction.phase = object[keys::condensed_phase].as<std::string>();
    reaction.solvent = object[keys::solvent].as<std::string>();
    reaction.reactants = ParseReactionComponents(object, keys::reactants);
    reaction.products = ParseReactionComponents(object, keys::products);
    reaction.rate_constant = ParseRateConstant(object[keys::rate_constant]);

    return reaction;
  }

  types::DissolvedReversibleReaction ParseDissolvedReversibleReaction(const YAML::Node& object)
  {
    types::DissolvedReversibleReaction reaction;

    reaction.phase = object[keys::condensed_phase].as<std::string>();
    reaction.solvent = object[keys::solvent].as<std::string>();
    reaction.reactants = ParseReactionComponents(object, keys::reactants);
    reaction.products = ParseReactionComponents(object, keys::products);

    // Any two of {forward, reverse, equilibrium} may be supplied; the third is derived
    // downstream, so each is parsed only when present.
    if (object[keys::forward_rate_constant])
      reaction.forward_rate_constant = ParseRateConstant(object[keys::forward_rate_constant]);
    if (object[keys::reverse_rate_constant])
      reaction.reverse_rate_constant = ParseRateConstant(object[keys::reverse_rate_constant]);
    if (object[keys::equilibrium_constant])
      reaction.equilibrium_constant = ParseEquilibrium(object[keys::equilibrium_constant]);

    return reaction;
  }

  // ----------------------------------------
  // Constraint parsers
  // ----------------------------------------

  types::HenrysLawEquilibrium ParseHenrysLawEquilibrium(
      const YAML::Node& object,
      const std::vector<types::Species>& species,
      const std::vector<types::Phase>& phases)
  {
    types::HenrysLawEquilibrium equilibrium;

    equilibrium.gas_phase = object[keys::gas_phase].as<std::string>();
    equilibrium.gas_species = object[keys::gas_phase_species].as<std::string>();
    equilibrium.condensed_phase = object[keys::condensed_phase].as<std::string>();
    equilibrium.condensed_species = object[keys::condensed_phase_species].as<std::string>();
    equilibrium.solvent = object[keys::solvent].as<std::string>();
    equilibrium.henrys_law_constant = ParseHenrysLawConstant(object[keys::henrys_law_constant]);

    // Sourced from the solvent species' definition: molecular weight from the species section,
    // density from the condensed phase. Presence is enforced by ValidateAerosolSemantics, which
    // runs before this parser is ever reached (in Parser::ValidateAndBuild), so a missing value
    // defaults harmlessly here.
    equilibrium.solvent_molecular_weight = FindSpeciesMolecularWeight(species, equilibrium.solvent).value_or(0.0);
    equilibrium.solvent_density =
        FindPhaseSpeciesDensity(phases, equilibrium.condensed_phase, equilibrium.solvent).value_or(0.0);

    return equilibrium;
  }

  types::DissolvedEquilibrium ParseDissolvedEquilibrium(const YAML::Node& object)
  {
    types::DissolvedEquilibrium equilibrium;

    equilibrium.phase = object[keys::condensed_phase].as<std::string>();
    equilibrium.algebraic_species = object[keys::algebraic_species].as<std::string>();
    equilibrium.solvent = object[keys::solvent].as<std::string>();
    equilibrium.reactants = ParseReactionComponents(object, keys::reactants);
    equilibrium.products = ParseReactionComponents(object, keys::products);
    equilibrium.equilibrium_constant = ParseEquilibrium(object[keys::equilibrium_constant]);

    return equilibrium;
  }

  types::LinearConstraint ParseLinearConstraint(const YAML::Node& object)
  {
    types::LinearConstraint constraint;

    constraint.algebraic_phase = object[keys::algebraic_phase].as<std::string>();
    constraint.algebraic_species = object[keys::algebraic_species].as<std::string>();

    for (const auto& term_node : object[keys::terms])
    {
      types::LinearConstraintTerm term;
      term.phase = term_node[keys::phase].as<std::string>();
      term.name = term_node[keys::name].as<std::string>();
      term.coefficient = term_node[keys::coefficient].as<double>();
      constraint.terms.push_back(term);
    }

    // RHS constant: diagnose-from-state, else a fixed value, else the default FixedConstant{0}.
    if (object[keys::diagnose_from_state] && object[keys::diagnose_from_state].as<bool>())
      constraint.constant = types::DiagnoseFromState{};
    else if (object[keys::constant])
      constraint.constant = types::FixedConstant{ object[keys::constant].as<double>() };

    return constraint;
  }

  // ----------------------------------------
  // Container parsers
  // ----------------------------------------

  std::vector<types::Representation> ParseAerosolRepresentations(const YAML::Node& objects)
  {
    std::vector<types::Representation> representations;

    for (const auto& object : objects)
    {
      const auto type = object[keys::type].as<std::string>();

      if (type == keys::UniformSection_key)
        representations.emplace_back(ParseUniformSection(object));
      else if (type == keys::SingleMomentMode_key)
        representations.emplace_back(ParseSingleMomentMode(object));
      else if (type == keys::TwoMomentMode_key)
        representations.emplace_back(ParseTwoMomentMode(object));
    }

    return representations;
  }

  types::Aerosol
  ParseAerosol(const YAML::Node& object, const std::vector<types::Species>& species, const std::vector<types::Phase>& phases)
  {
    types::Aerosol aerosol;

    if (object[keys::aerosol_representations])
      aerosol.representations = ParseAerosolRepresentations(object[keys::aerosol_representations]);

    if (object[keys::aerosol_processes])
    {
      // Aerosol processes section mixes process and constraint entries.
      for (const auto& entry : object[keys::aerosol_processes])
      {
        const auto type = entry[keys::type].as<std::string>();

        // Processes
        if (type == keys::HenrysLawPhaseTransfer_key)
          aerosol.processes.emplace_back(ParseHenrysLawPhaseTransfer(entry, phases));
        else if (type == keys::DissolvedReaction_key)
          aerosol.processes.emplace_back(ParseDissolvedReaction(entry));
        else if (type == keys::DissolvedReversibleReaction_key)
          aerosol.processes.emplace_back(ParseDissolvedReversibleReaction(entry));
        // Constraints
        else if (type == keys::HenrysLawEquilibrium_key)
          aerosol.constraints.emplace_back(ParseHenrysLawEquilibrium(entry, species, phases));
        else if (type == keys::DissolvedEquilibrium_key)
          aerosol.constraints.emplace_back(ParseDissolvedEquilibrium(entry));
        else if (type == keys::LinearConstraint_key)
          aerosol.constraints.emplace_back(ParseLinearConstraint(entry));
      }
    }

    return aerosol;
  }

}  // namespace mechanism_configuration::v1

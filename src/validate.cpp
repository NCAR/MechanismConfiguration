// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/validate.hpp>

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace mechanism_configuration
{
  namespace
  {
    // Formats a message, prefixing "line:col error:" when a source location is known.
    std::string Message(const std::optional<ErrorLocation>& location, const std::string& body)
    {
      if (location)
        return mc_fmt::format("{} error: {}", *location, body);
      return "error: " + body;
    }

    // Emits one error per occurrence of each name that appears more than once.
    void
    ReportDuplicates(const std::vector<semantics::NamedRef>& refs, ErrorCode code, std::string_view what, Errors& errors)
    {
      std::unordered_map<std::string, int> counts;
      for (const auto& ref : refs)
        ++counts[ref.name];
      for (const auto& ref : refs)
      {
        if (counts[ref.name] > 1)
          errors.push_back({ code, Message(ref.location, mc_fmt::format("Duplicate {} name '{}' found.", what, ref.name)) });
      }
    }
  }  // namespace

  Errors ValidateSemantics(const semantics::Input& input)
  {
    Errors errors;

    // ---- Species ----------------------------------------------------------------------------
    std::unordered_set<std::string> species_names;
    for (const auto& s : input.species)
      species_names.insert(s.name);
    ReportDuplicates(input.species, ErrorCode::DuplicateSpeciesDetected, "species", errors);

    // ---- Phases -----------------------------------------------------------------------------
    std::unordered_map<std::string, std::unordered_set<std::string>> phase_species;
    std::vector<semantics::NamedRef> phase_names;
    for (const auto& phase : input.phases)
    {
      phase_names.push_back({ phase.name, phase.location });
      auto& registered = phase_species[phase.name];
      for (const auto& ps : phase.species)
      {
        registered.insert(ps.name);
        if (!species_names.contains(ps.name))
          errors.push_back(
              { ErrorCode::PhaseRequiresUnknownSpecies,
                Message(
                    ps.location, mc_fmt::format("Unknown species name '{}' found in '{}' phase.", ps.name, phase.name)) });
      }
      ReportDuplicates(phase.species, ErrorCode::DuplicateSpeciesInPhaseDetected, "species", errors);
    }
    ReportDuplicates(phase_names, ErrorCode::DuplicatePhasesDetected, "phase", errors);

    // ---- Reactions --------------------------------------------------------------------------
    for (const auto& reaction : input.reactions)
    {
      const auto phase_it = phase_species.find(reaction.phase);
      const bool phase_exists = phase_it != phase_species.end();
      if (!phase_exists)
        errors.push_back({ ErrorCode::UnknownPhase,
                           Message(
                               reaction.location,
                               mc_fmt::format("Unknown phase '{}' in '{}' reaction.", reaction.phase, reaction.type)) });

      for (const auto& reactant : reaction.reactants)
      {
        if (!species_names.contains(reactant.name))
          errors.push_back(
              { ErrorCode::ReactionRequiresUnknownSpecies,
                Message(
                    reactant.location,
                    mc_fmt::format("Unknown species '{}' used in '{}' reaction.", reactant.name, reaction.type)) });
        else if (phase_exists && !phase_it->second.contains(reactant.name))
          errors.push_back({ ErrorCode::RequestedSpeciesNotRegisteredInPhase,
                             Message(
                                 reactant.location,
                                 mc_fmt::format(
                                     "Species '{}' used in '{}' is not defined in the '{}' phase.",
                                     reactant.name,
                                     reaction.type,
                                     reaction.phase)) });
      }
      for (const auto& product : reaction.products)
      {
        if (!species_names.contains(product.name))
          errors.push_back(
              { ErrorCode::ReactionRequiresUnknownSpecies,
                Message(
                    product.location,
                    mc_fmt::format("Unknown species '{}' used in '{}' reaction.", product.name, reaction.type)) });
      }
    }

    // ---- Aerosol Representations --------------------------------------------------------------------------
    for (const auto& representation : input.aerosol_representations)
    {
      for (const auto& phase : representation.phases)
      {
        if (!phase_species.contains(phase.name))
          errors.push_back(
              { ErrorCode::UnknownPhase,
                Message(
                    phase.location,
                    mc_fmt::format(
                        "Unknown phase '{}' in '{}' aerosol representation.", phase.name, representation.name)) });
      }
    }

    // ---- Aerosol Processes --------------------------------------------------------------------------
    for (const auto& process : input.aerosol_processes)
    {
      for (const auto& species : process.species)
      {
        if (!species_names.contains(species.name))
          errors.push_back(
              { ErrorCode::UnknownSpecies,
                Message(
                    species.location,
                    mc_fmt::format("Unknown species '{}' in '{}' aerosol process.", species.name, process.type)) });
      }
      for (const auto& phase : process.phases)
      {
        if (!phase_species.contains(phase.name))
          errors.push_back(
              { ErrorCode::UnknownPhase,
                Message(
                    phase.location,
                    mc_fmt::format("Unknown phase '{}' in '{}' aerosol process.", phase.name, process.type)) });
      }
    }

    return errors;
  }

  namespace
  {
    // Builds a location-free list of component references from a struct component list.
    std::vector<semantics::NamedRef> Refs(const std::vector<types::ReactionComponent>& components)
    {
      std::vector<semantics::NamedRef> refs;
      refs.reserve(components.size());
      for (const auto& c : components)
        refs.push_back({ c.name, std::nullopt });
      return refs;
    }

    semantics::ReactionRef ReactionRefOf(
        std::string_view type,
        const std::string& phase,
        std::vector<semantics::NamedRef> reactants,
        std::vector<semantics::NamedRef> products)
    {
      return semantics::ReactionRef{ std::string(type), phase, std::move(reactants), std::move(products), std::nullopt };
    }
  }  // namespace

  Errors Validate(const Mechanism& mechanism)
  {
    semantics::Input input;

    for (const auto& s : mechanism.species)
      input.species.push_back({ s.name, std::nullopt });

    for (const auto& phase : mechanism.phases)
    {
      semantics::PhaseRef pr{ phase.name, {}, std::nullopt };
      for (const auto& ps : phase.species)
        pr.species.push_back({ ps.name, std::nullopt });
      input.phases.push_back(std::move(pr));
    }

    // ---- Reactions --------------------------------------------------------------------------
    const auto& r = mechanism.reactions;
    auto add = [&](std::string_view type,
                   const std::string& phase,
                   std::vector<semantics::NamedRef> reactants,
                   std::vector<semantics::NamedRef> products)
    { input.reactions.push_back(ReactionRefOf(type, phase, std::move(reactants), std::move(products))); };

    for (const auto& x : r.arrhenius)
      add("ARRHENIUS", x.gas_phase, Refs(x.reactants), Refs(x.products));
    for (const auto& x : r.troe)
      add("TROE", x.gas_phase, Refs(x.reactants), Refs(x.products));
    for (const auto& x : r.ternary_chemical_activation)
      add("TERNARY_CHEMICAL_ACTIVATION", x.gas_phase, Refs(x.reactants), Refs(x.products));
    for (const auto& x : r.tunneling)
      add("TUNNELING", x.gas_phase, Refs(x.reactants), Refs(x.products));
    for (const auto& x : r.taylor_series)
      add("TAYLOR_SERIES", x.gas_phase, Refs(x.reactants), Refs(x.products));
    for (const auto& x : r.user_defined)
      add("USER_DEFINED", x.gas_phase, Refs(x.reactants), Refs(x.products));
    for (const auto& x : r.lambda_rate_constant)
      add("LAMBDA_RATE_CONSTANT", x.gas_phase, Refs(x.reactants), Refs(x.products));
    for (const auto& x : r.emission)
      add("EMISSION", x.gas_phase, {}, Refs(x.products));
    for (const auto& x : r.first_order_loss)
      add("FIRST_ORDER_LOSS", x.gas_phase, Refs({ x.reactants }), Refs(x.products));
    for (const auto& x : r.photolysis)
      add("PHOTOLYSIS", x.gas_phase, Refs({ x.reactants }), Refs(x.products));
    for (const auto& x : r.surface)
      add("SURFACE", x.gas_phase, Refs({ x.gas_phase_species }), Refs(x.gas_phase_products));
    for (const auto& x : r.branched)
    {
      std::vector<types::ReactionComponent> products = x.alkoxy_products;
      products.insert(products.end(), x.nitrate_products.begin(), x.nitrate_products.end());
      add("BRANCHED", x.gas_phase, Refs(x.reactants), Refs(products));
    }

    // ---- Aerosol Representations --------------------------------------------------------------------------
    const auto& a = mechanism.aerosol;
    for (const auto& x : a.representations)
    {
      std::visit(
          [&](const auto& rep)
          {
            using T = std::decay_t<decltype(rep)>;
            std::string_view type;
            if constexpr (std::is_same_v<T, types::UniformSection>)
              type = "UNIFORM_SECTION";
            else if constexpr (std::is_same_v<T, types::SingleMomentMode>)
              type = "SINGLE_MOMENT_MODE";
            else if constexpr (std::is_same_v<T, types::TwoMomentMode>)
              type = "TWO_MOMENT_MODE";

            semantics::AerosolRepresentationRef ref{ std::string(type), rep.name, {}, std::nullopt };
            for (const auto& phase : rep.phases)
              ref.phases.push_back({ phase, std::nullopt });
            input.aerosol_representations.push_back(std::move(ref));
          },
          x);
    }

    // ---- Aerosol Processes --------------------------------------------------------------------------
    for (const auto& x : a.processes)
    {
      std::visit(
          [&](const auto& proc)
          {
            using T = std::decay_t<decltype(proc)>;
            semantics::AerosolProcessRef ref;

            if constexpr (std::is_same_v<T, types::DissolvedReaction>)
            {
              ref.type = "DISSOLVED_REACTION";
              ref.phases.push_back({ proc.phase, std::nullopt });
              ref.species.push_back({ proc.solvent, std::nullopt });
              for (const auto& r : proc.reactants)
                ref.species.push_back({ r.name, std::nullopt });
              for (const auto& p : proc.products)
                ref.species.push_back({ p.name, std::nullopt });
              std::vector<semantics::NamedRef> rep_refs;
              for (const auto& [rep_name, _] : proc.rate_constants)
                rep_refs.push_back({ rep_name, std::nullopt });
              ref.aerosol_representations = std::move(rep_refs);
            }
            else if constexpr (std::is_same_v<T, types::DissolvedReversibleReaction>)
            {
              ref.type = "DISSOLVED_REVERSIBLE_REACTION";
              ref.phases.push_back({ proc.phase, std::nullopt });
              ref.species.push_back({ proc.solvent, std::nullopt });
              for (const auto& r : proc.reactants)
                ref.species.push_back({ r.name, std::nullopt });
              for (const auto& p : proc.products)
                ref.species.push_back({ p.name, std::nullopt });
              std::vector<semantics::NamedRef> rep_refs;
              for (const auto& [rep_name, _] : proc.forward_rate_constants)
                rep_refs.push_back({ rep_name, std::nullopt });
              for (const auto& [rep_name, _] : proc.reverse_rate_constants)
                rep_refs.push_back({ rep_name, std::nullopt });
              ref.aerosol_representations = std::move(rep_refs);
            }
            else if constexpr (std::is_same_v<T, types::HenryLawPhaseTransfer>)
            {
              ref.type = "HENRY_LAW_PHASE_TRANSFER";
              ref.phases.push_back({ proc.gas_phase, std::nullopt });
              ref.phases.push_back({ proc.condensed_phase, std::nullopt });
              ref.species.push_back({ proc.gas_species, std::nullopt });
              ref.species.push_back({ proc.condensed_species, std::nullopt });
              ref.species.push_back({ proc.solvent, std::nullopt });
            }

            input.aerosol_processes.push_back(std::move(ref));
          },
          x);
    }

    // ---- Aerosol Constraints --------------------------------------------------------------------------
    for (const auto& x : a.constraints)
    {
      std::visit(
          [&](const auto& con)
          {
            using T = std::decay_t<decltype(con)>;
            semantics::AerosolProcessRef ref;

            if constexpr (std::is_same_v<T, types::HenryLawEquilibrium>)
            {
              ref.type = "HENRY_LAW_EQUILIBRIUM";
              ref.phases.push_back({ con.gas_phase, std::nullopt });
              ref.phases.push_back({ con.condensed_phase, std::nullopt });
              ref.species.push_back({ con.gas_species, std::nullopt });
              ref.species.push_back({ con.condensed_species, std::nullopt });
              ref.species.push_back({ con.solvent, std::nullopt });
            }
            else if constexpr (std::is_same_v<T, types::DissolvedEquilibrium>)
            {
              ref.type = "DISSOLVED_EQUILIBRIUM";
              ref.phases.push_back({ con.phase, std::nullopt });
              ref.species.push_back({ con.algebraic_species, std::nullopt });
              ref.species.push_back({ con.solvent, std::nullopt });
              for (const auto& r : con.reactants)
                ref.species.push_back({ r.name, std::nullopt });
              for (const auto& p : con.products)
                ref.species.push_back({ p.name, std::nullopt });
            }
            else if constexpr (std::is_same_v<T, types::LinearConstraint>)
            {
              ref.type = "LINEAR_CONSTRAINT";
              ref.phases.push_back({ con.algebraic_phase, std::nullopt });
              ref.species.push_back({ con.algebraic_species, std::nullopt });
              for (const auto& term : con.terms)
              {
                ref.phases.push_back({ term.phase, std::nullopt });
                ref.species.push_back({ term.name, std::nullopt });
              }
            }

            input.aerosol_processes.push_back(std::move(ref));
          },
          x);
    }

    Errors errors = ValidateSemantics(input);
    // Errors aerosol_errors = ValidateAerosol(mechanism);
    // errors.insert(errors.end(), aerosol_errors.begin(), aerosol_errors.end());
    return errors;
  }

}  // namespace mechanism_configuration

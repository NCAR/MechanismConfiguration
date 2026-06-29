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

  Errors ValidateGasModel(const Mechanism& mechanism)
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

    return ValidateSemantics(input);
  }

  Errors ValidateAerosolModel(const Mechanism& mechanism)
  {
    Errors errors;

    const auto& aerosol = mechanism.aerosol;
    if (aerosol.representations.empty() && aerosol.processes.empty() && aerosol.constraints.empty())
      return errors;

    // Index species for molecular weight.
    std::unordered_map<std::string, const types::Species*> species_index;
    for (const auto& s : mechanism.species)
      species_index.emplace(s.name, &s);

    // Index phase-species for membership, diffusion, density.
    std::unordered_map<std::string, std::unordered_map<std::string, const types::PhaseSpecies*>> phase_index;
    for (const auto& phase : mechanism.phases)
    {
      auto& registered = phase_index[phase.name];
      for (const auto& ps : phase.species)
        registered.emplace(ps.name, &ps);
    }

    // Rate-constants are keyed by its aerosol representation
    std::unordered_set<std::string> representation_names;
    for (const auto& representation : aerosol.representations)
      std::visit([&](const auto& rep) { representation_names.insert(rep.name); }, representation);

    // Verifies that species is registered in phase. Returns the entry (or nullptr) and reports.
    auto require_registered_species =
        [&](const std::string& phase, const std::string& species, const std::string& context) -> const types::PhaseSpecies*
    {
      const auto phase_it = phase_index.find(phase);
      if (phase_it == phase_index.end())
      {
        errors.push_back({ ErrorCode::UnknownPhase,
                           Message(std::nullopt, mc_fmt::format("Unknown phase '{}' referenced by {}.", phase, context)) });
        return nullptr;
      }
      const auto species_it = phase_it->second.find(species);
      if (species_it == phase_it->second.end())
      {
        errors.push_back({ ErrorCode::RequestedSpeciesNotRegisteredInPhase,
                           Message(std::nullopt,
                                   mc_fmt::format(
                                       "Species '{}' ({}) is not defined in the '{}' phase.", species, context, phase)) });
        return nullptr;
      }
      return species_it->second;
    };

    auto require_diffusion = [&](const std::string& phase, const std::string& species, const std::string& context)
    {
      const auto* entry = require_registered_species(phase, species, context);
      if (entry && !entry->diffusion_coefficient)
        errors.push_back({ ErrorCode::RequiredKeyNotFound,
                           Message(std::nullopt,
                                   mc_fmt::format("{}: species '{}' has no diffusion coefficient defined in "
                                                  "the '{}' phase.",
                                                  context, species, phase)) });
    };

    auto require_density = [&](const std::string& phase, const std::string& species, const std::string& context)
    {
      const auto* entry = require_registered_species(phase, species, context);
      if (entry && !entry->density)
        errors.push_back({ ErrorCode::RequiredKeyNotFound,
                           Message(std::nullopt,
                                   mc_fmt::format("{}: species '{}' has no density defined in the '{}' phase.",
                                                  context, species, phase)) });
    };

    auto require_molecular_weight = [&](const std::string& species, const std::string& context)
    {
      const auto species_it = species_index.find(species);
      if (species_it == species_index.end())
        errors.push_back({ ErrorCode::UnknownSpecies,
                           Message(std::nullopt, mc_fmt::format("Unknown species '{}' referenced by {}.", species, context)) });
      else if (!species_it->second->molecular_weight)
        errors.push_back({ ErrorCode::RequiredKeyNotFound,
                           Message(std::nullopt,
                                   mc_fmt::format("{}: species '{}' has no molecular weight defined.", context, species)) });
    };

    // Verifies that phase exists.
    auto require_phase = [&](const std::string& phase, const std::string& context)
    {
      if (!phase_index.contains(phase))
        errors.push_back({ ErrorCode::UnknownPhase,
                           Message(std::nullopt, mc_fmt::format("Unknown phase '{}' referenced by {}.", phase, context)) });
    };

    // Validates that each rate-constant map entry is keyed by a declared aerosol representation.
    auto require_representation = [&](const std::string& representation, const std::string& context)
    {
      if (!representation_names.contains(representation))
        errors.push_back({ ErrorCode::UnknownAerosolRepresentation,
                           Message(std::nullopt,
                                   mc_fmt::format(
                                       "Unknown aerosol representation '{}' referenced by {}.", representation, context)) });
    };

    for (const auto& representation : aerosol.representations)
    {
      std::visit(
          [&](const auto& rep)
          {
            for (const auto& phase : rep.phases)
              require_phase(phase, mc_fmt::format("aerosol representation '{}'", rep.name));
          },
          representation);
    }

    for (const auto& process : aerosol.processes)
    {
      if (const auto* p = std::get_if<types::DissolvedReaction>(&process))
      {
        for (const auto& c : p->reactants)
          require_registered_species(p->phase, c.name, "DISSOLVED_REACTION reactant");
        for (const auto& c : p->products)
          require_registered_species(p->phase, c.name, "DISSOLVED_REACTION product");
        require_registered_species(p->phase, p->solvent, "DISSOLVED_REACTION solvent");
        for (const auto& entry : p->rate_constants)
          require_representation(entry.first, "DISSOLVED_REACTION rate constant");
      }
      else if (const auto* p = std::get_if<types::DissolvedReversibleReaction>(&process))
      {
        for (const auto& c : p->reactants)
          require_registered_species(p->phase, c.name, "DISSOLVED_REVERSIBLE_REACTION reactant");
        for (const auto& c : p->products)
          require_registered_species(p->phase, c.name, "DISSOLVED_REVERSIBLE_REACTION product");
        require_registered_species(p->phase, p->solvent, "DISSOLVED_REVERSIBLE_REACTION solvent");
        for (const auto& entry : p->forward_rate_constants)
          require_representation(entry.first, "DISSOLVED_REVERSIBLE_REACTION forward rate constant");
        for (const auto& entry : p->reverse_rate_constants)
          require_representation(entry.first, "DISSOLVED_REVERSIBLE_REACTION reverse rate constant");
      }
      else if (const auto* p = std::get_if<types::HenryLawPhaseTransfer>(&process))
      {
        require_registered_species(p->condensed_phase, p->condensed_species, "HENRY_LAW_PHASE_TRANSFER condensed species");
        require_registered_species(p->condensed_phase, p->solvent, "HENRY_LAW_PHASE_TRANSFER solvent");
        require_diffusion(p->gas_phase, p->gas_species, "HENRY_LAW_PHASE_TRANSFER");
      }
    }

    for (const auto& constraint : aerosol.constraints)
    {
      if (const auto* c = std::get_if<types::HenryLawEquilibrium>(&constraint))
      {
        require_registered_species(c->gas_phase, c->gas_species, "HENRY_LAW_EQUILIBRIUM gas species");
        require_registered_species(c->condensed_phase, c->condensed_species, "HENRY_LAW_EQUILIBRIUM condensed species");
        require_density(c->condensed_phase, c->solvent, "HENRY_LAW_EQUILIBRIUM solvent");
        require_molecular_weight(c->solvent, "HENRY_LAW_EQUILIBRIUM solvent");
      }
      else if (const auto* c = std::get_if<types::DissolvedEquilibrium>(&constraint))
      {
        require_registered_species(c->phase, c->algebraic_species, "DISSOLVED_EQUILIBRIUM algebraic species");
        require_registered_species(c->phase, c->solvent, "DISSOLVED_EQUILIBRIUM solvent");
        for (const auto& x : c->reactants)
          require_registered_species(c->phase, x.name, "DISSOLVED_EQUILIBRIUM reactant");
        for (const auto& x : c->products)
          require_registered_species(c->phase, x.name, "DISSOLVED_EQUILIBRIUM product");
      }
      else if (const auto* c = std::get_if<types::LinearConstraint>(&constraint))
      {
        require_registered_species(c->algebraic_phase, c->algebraic_species, "LINEAR_CONSTRAINT algebraic species");
        for (const auto& t : c->terms)
          require_registered_species(t.phase, t.name, "LINEAR_CONSTRAINT term");
      }
    }

    return errors;
  }

  Errors Validate(const Mechanism& mechanism)
  {
    Errors errors = ValidateGasModel(mechanism);
    Errors aerosol_errors = ValidateAerosolModel(mechanism);
    errors.insert(errors.end(), aerosol_errors.begin(), aerosol_errors.end());
    return errors;
  }

}  // namespace mechanism_configuration

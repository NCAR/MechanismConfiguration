// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/error_format.hpp"
#include "detail/semantics/aerosol.hpp"
#include "detail/semantics/emissions.hpp"
#include "detail/semantics/reactions.hpp"

#include <mechanism_configuration/validate.hpp>

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
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

  Errors ValidateReactionsSemantics(const semantics::ReactionsInput& input)
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

  Errors ValidateAerosolSemantics(const semantics::AerosolInput& input)
  {
    Errors errors;

    if (input.representations.empty() && input.dissolved_reactions.empty() && input.dissolved_reversible_reactions.empty() &&
        input.henrys_law_phase_transfers.empty() && input.henrys_law_equilibria.empty() &&
        input.dissolved_equilibria.empty() && input.linear_constraints.empty())
      return errors;

    // Index species for molecular weight.
    std::unordered_map<std::string, const semantics::SpeciesDef*> species_index;
    for (const auto& s : input.species)
      species_index.emplace(s.name, &s);

    // Index phase-species for membership, diffusion, density.
    std::unordered_map<std::string, std::unordered_map<std::string, const semantics::PhaseSpeciesDef*>> phase_index;
    for (const auto& phase : input.phases)
    {
      auto& registered = phase_index[phase.name];
      for (const auto& ps : phase.species)
        registered.emplace(ps.name, &ps);
    }

    // Verifies that species is registered in phase. Returns the entry (or nullptr) and reports.
    auto require_registered_species = [&](const semantics::NamedRef& phase,
                                          const semantics::NamedRef& species,
                                          const std::string& context) -> const semantics::PhaseSpeciesDef*
    {
      const auto phase_it = phase_index.find(phase.name);
      if (phase_it == phase_index.end())
      {
        errors.push_back(
            { ErrorCode::UnknownPhase,
              Message(phase.location, mc_fmt::format("Unknown phase '{}' referenced by {}.", phase.name, context)) });
        return nullptr;
      }
      const auto species_it = phase_it->second.find(species.name);
      if (species_it == phase_it->second.end())
      {
        errors.push_back(
            { ErrorCode::RequestedSpeciesNotRegisteredInPhase,
              Message(
                  species.location,
                  mc_fmt::format(
                      "Species '{}' ({}) is not defined in the '{}' phase.", species.name, context, phase.name)) });
        return nullptr;
      }
      return species_it->second;
    };

    auto require_diffusion =
        [&](const semantics::NamedRef& phase, const semantics::NamedRef& species, const std::string& context)
    {
      const auto* entry = require_registered_species(phase, species, context);
      if (entry && !entry->has_diffusion_coefficient)
        errors.push_back({ ErrorCode::RequiredKeyNotFound,
                           Message(
                               species.location,
                               mc_fmt::format(
                                   "{}: species '{}' has no diffusion coefficient defined in "
                                   "the '{}' phase.",
                                   context,
                                   species.name,
                                   phase.name)) });
    };

    auto require_density =
        [&](const semantics::NamedRef& phase, const semantics::NamedRef& species, const std::string& context)
    {
      const auto* entry = require_registered_species(phase, species, context);
      if (entry && !entry->has_density)
        errors.push_back(
            { ErrorCode::RequiredKeyNotFound,
              Message(
                  species.location,
                  mc_fmt::format(
                      "{}: species '{}' has no density defined in the '{}' phase.", context, species.name, phase.name)) });
    };

    auto require_molecular_weight = [&](const semantics::NamedRef& species, const std::string& context)
    {
      const auto species_it = species_index.find(species.name);
      if (species_it == species_index.end())
        errors.push_back(
            { ErrorCode::UnknownSpecies,
              Message(species.location, mc_fmt::format("Unknown species '{}' referenced by {}.", species.name, context)) });
      else if (!species_it->second->has_molecular_weight)
        errors.push_back(
            { ErrorCode::RequiredKeyNotFound,
              Message(
                  species.location,
                  mc_fmt::format("{}: species '{}' has no molecular weight defined.", context, species.name)) });
    };

    // Verifies that phase exists.
    auto require_phase = [&](const semantics::NamedRef& phase, const std::string& context)
    {
      if (!phase_index.contains(phase.name))
        errors.push_back(
            { ErrorCode::UnknownPhase,
              Message(phase.location, mc_fmt::format("Unknown phase '{}' referenced by {}.", phase.name, context)) });
    };

    for (const auto& representation : input.representations)
      for (const auto& phase : representation.phases)
        require_phase(phase, mc_fmt::format("aerosol representation '{}'", representation.name));

    for (const auto& p : input.dissolved_reactions)
    {
      for (const auto& c : p.reactants)
        require_registered_species(p.phase, c, "DISSOLVED_REACTION reactant");
      for (const auto& c : p.products)
        require_registered_species(p.phase, c, "DISSOLVED_REACTION product");
      require_registered_species(p.phase, p.solvent, "DISSOLVED_REACTION solvent");
    }

    for (const auto& p : input.dissolved_reversible_reactions)
    {
      for (const auto& c : p.reactants)
        require_registered_species(p.phase, c, "DISSOLVED_REVERSIBLE_REACTION reactant");
      for (const auto& c : p.products)
        require_registered_species(p.phase, c, "DISSOLVED_REVERSIBLE_REACTION product");
      require_registered_species(p.phase, p.solvent, "DISSOLVED_REVERSIBLE_REACTION solvent");
    }

    for (const auto& p : input.henrys_law_phase_transfers)
    {
      require_registered_species(p.condensed_phase, p.condensed_species, "HENRYS_LAW_PHASE_TRANSFER condensed species");
      require_registered_species(p.condensed_phase, p.solvent, "HENRYS_LAW_PHASE_TRANSFER solvent");
      require_diffusion(p.gas_phase, p.gas_species, "HENRYS_LAW_PHASE_TRANSFER");
    }

    for (const auto& c : input.henrys_law_equilibria)
    {
      require_registered_species(c.gas_phase, c.gas_species, "HENRYS_LAW_EQUILIBRIUM gas species");
      require_registered_species(c.condensed_phase, c.condensed_species, "HENRYS_LAW_EQUILIBRIUM condensed species");
      require_density(c.condensed_phase, c.solvent, "HENRYS_LAW_EQUILIBRIUM solvent");
      require_molecular_weight(c.solvent, "HENRYS_LAW_EQUILIBRIUM solvent");
    }

    for (const auto& c : input.dissolved_equilibria)
    {
      require_registered_species(c.phase, c.algebraic_species, "DISSOLVED_EQUILIBRIUM algebraic species");
      require_registered_species(c.phase, c.solvent, "DISSOLVED_EQUILIBRIUM solvent");
      for (const auto& x : c.reactants)
        require_registered_species(c.phase, x, "DISSOLVED_EQUILIBRIUM reactant");
      for (const auto& x : c.products)
        require_registered_species(c.phase, x, "DISSOLVED_EQUILIBRIUM product");
    }

    for (const auto& c : input.linear_constraints)
    {
      require_registered_species(c.algebraic_phase, c.algebraic_species, "LINEAR_CONSTRAINT algebraic species");
      for (const auto& t : c.terms)
        require_registered_species(t.phase, t.species, "LINEAR_CONSTRAINT term");
    }

    return errors;
  }

  Errors ValidateEmissionsSemantics(const semantics::EmissionsInput& input)
  {
    Errors errors;

    ReportDuplicates(input.inventories, ErrorCode::DuplicateInventoryDetected, "inventory", errors);

    std::vector<semantics::NamedRef> species_map_names;
    for (const auto& smap : input.species_maps)
      species_map_names.push_back({ smap.name, smap.location });
    ReportDuplicates(species_map_names, ErrorCode::DuplicateSpeciesMapDetected, "species map", errors);

    std::vector<semantics::NamedRef> source_names;
    for (const auto& source : input.sources)
      source_names.push_back({ source.name, source.location });
    ReportDuplicates(source_names, ErrorCode::DuplicateSourceDetected, "source", errors);

    std::unordered_set<std::string> inventory_names;
    for (const auto& inv : input.inventories)
      inventory_names.insert(inv.name);

    std::unordered_set<std::string> species_map_name_set;
    for (const auto& smap : input.species_maps)
      species_map_name_set.insert(smap.name);

    std::map<std::pair<int, int>, int> cat_hier_counts;
    for (const auto& source : input.sources)
      ++cat_hier_counts[{ source.category, source.hierarchy }];

    for (const auto& source : input.sources)
    {
      if (!inventory_names.contains(source.inventory.name))
        errors.push_back({ ErrorCode::SourceRequiresUnknownInventory,
                           Message(
                               source.inventory.location,
                               mc_fmt::format(
                                   "Source '{}' references inventory '{}' which is not declared in 'inventories'.",
                                   source.name,
                                   source.inventory.name)) });

      if (!species_map_name_set.contains(source.species_map.name))
        errors.push_back({ ErrorCode::SourceRequiresUnknownSpeciesMap,
                           Message(
                               source.species_map.location,
                               mc_fmt::format(
                                   "Source '{}' references species map '{}' which is not declared in 'species maps'.",
                                   source.name,
                                   source.species_map.name)) });

      if (cat_hier_counts[{ source.category, source.hierarchy }] > 1)
        errors.push_back(
            { ErrorCode::DuplicateCategoryHierarchy,
              Message(
                  source.location,
                  mc_fmt::format(
                      "Source '{}' has duplicate (category: {}, hierarchy: {}) — each (category, hierarchy) pair "
                      "must be unique.",
                      source.name,
                      source.category,
                      source.hierarchy)) });
    }

    for (const auto& smap : input.species_maps)
    {
      std::unordered_map<std::string, double> scale_sum;
      for (const auto& mapping : smap.mappings)
        scale_sum[mapping.inventory_species] += mapping.scaling_factor;

      for (const auto& [inventory_species, total] : scale_sum)
      {
        if (total > 1.0 + 1e-9)
          errors.push_back(
              { ErrorCode::SpeciesMapScalingExceedsOne,
                Message(
                    smap.location,
                    mc_fmt::format(
                        "Species map '{}': scaling factors for inventory species '{}' sum to {:.4f}, which exceeds "
                        "1.0.",
                        smap.name,
                        inventory_species,
                        total)) });
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
    semantics::ReactionsInput input;

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

    return ValidateReactionsSemantics(input);
  }

  Errors ValidateAerosolModel(const Mechanism& mechanism)
  {
    if (!mechanism.aerosol)
      return {};

    semantics::AerosolInput input;

    for (const auto& s : mechanism.species)
      input.species.push_back({ s.name, s.molecular_weight.has_value() });

    for (const auto& phase : mechanism.phases)
    {
      semantics::PhaseDef phase_def;
      phase_def.name = phase.name;
      for (const auto& ps : phase.species)
        phase_def.species.push_back({ ps.name, ps.diffusion_coefficient.has_value(), ps.density.has_value() });
      input.phases.push_back(std::move(phase_def));
    }

    const auto& aerosol = *mechanism.aerosol;

    for (const auto& representation : aerosol.representations)
    {
      std::visit(
          [&](const auto& rep)
          {
            semantics::AerosolRepresentationRef ref;
            ref.name = rep.name;
            for (const auto& phase : rep.phases)
              ref.phases.push_back({ phase, std::nullopt });
            input.representations.push_back(std::move(ref));
          },
          representation);
    }

    for (const auto& process : aerosol.processes)
    {
      if (const auto* p = std::get_if<types::DissolvedReaction>(&process))
      {
        semantics::DissolvedReactionRef ref;
        ref.phase = { p->phase, std::nullopt };
        ref.solvent = { p->solvent, std::nullopt };
        ref.reactants = Refs(p->reactants);
        ref.products = Refs(p->products);
        input.dissolved_reactions.push_back(std::move(ref));
      }
      else if (const auto* p = std::get_if<types::DissolvedReversibleReaction>(&process))
      {
        semantics::DissolvedReversibleReactionRef ref;
        ref.phase = { p->phase, std::nullopt };
        ref.solvent = { p->solvent, std::nullopt };
        ref.reactants = Refs(p->reactants);
        ref.products = Refs(p->products);
        input.dissolved_reversible_reactions.push_back(std::move(ref));
      }
      else if (const auto* p = std::get_if<types::HenrysLawPhaseTransfer>(&process))
      {
        semantics::HenrysLawPhaseTransferRef ref;
        ref.gas_phase = { p->gas_phase, std::nullopt };
        ref.gas_species = { p->gas_species, std::nullopt };
        ref.condensed_phase = { p->condensed_phase, std::nullopt };
        ref.condensed_species = { p->condensed_species, std::nullopt };
        ref.solvent = { p->solvent, std::nullopt };
        input.henrys_law_phase_transfers.push_back(std::move(ref));
      }
    }

    for (const auto& constraint : aerosol.constraints)
    {
      if (const auto* c = std::get_if<types::HenrysLawEquilibrium>(&constraint))
      {
        semantics::HenrysLawEquilibriumRef ref;
        ref.gas_phase = { c->gas_phase, std::nullopt };
        ref.gas_species = { c->gas_species, std::nullopt };
        ref.condensed_phase = { c->condensed_phase, std::nullopt };
        ref.condensed_species = { c->condensed_species, std::nullopt };
        ref.solvent = { c->solvent, std::nullopt };
        input.henrys_law_equilibria.push_back(std::move(ref));
      }
      else if (const auto* c = std::get_if<types::DissolvedEquilibrium>(&constraint))
      {
        semantics::DissolvedEquilibriumRef ref;
        ref.phase = { c->phase, std::nullopt };
        ref.algebraic_species = { c->algebraic_species, std::nullopt };
        ref.solvent = { c->solvent, std::nullopt };
        ref.reactants = Refs(c->reactants);
        ref.products = Refs(c->products);
        input.dissolved_equilibria.push_back(std::move(ref));
      }
      else if (const auto* c = std::get_if<types::LinearConstraint>(&constraint))
      {
        semantics::LinearConstraintRef ref;
        ref.algebraic_phase = { c->algebraic_phase, std::nullopt };
        ref.algebraic_species = { c->algebraic_species, std::nullopt };
        for (const auto& t : c->terms)
          ref.terms.push_back({ { t.phase, std::nullopt }, { t.name, std::nullopt } });
        input.linear_constraints.push_back(std::move(ref));
      }
    }

    return ValidateAerosolSemantics(input);
  }

  Errors ValidateEmissionsModel(const Mechanism& mechanism)
  {
    if (!mechanism.emissions)
      return {};

    semantics::EmissionsInput input;

    for (const auto& inv : mechanism.emissions->inventories)
      input.inventories.push_back({ inv.name, std::nullopt });

    for (const auto& smap : mechanism.emissions->species_maps)
    {
      semantics::SpeciesMapRef smap_ref;
      smap_ref.name = smap.name;
      for (const auto& mapping : smap.mappings)
        smap_ref.mappings.push_back({ mapping.inventory_species, mapping.mechanism_species, mapping.scaling_factor });
      input.species_maps.push_back(std::move(smap_ref));
    }

    for (const auto& source : mechanism.emissions->sources)
    {
      semantics::SourceRef source_ref;
      source_ref.name = source.name;
      source_ref.inventory = { source.inventory, std::nullopt };
      source_ref.species_map = { source.species_map, std::nullopt };
      source_ref.category = source.category;
      source_ref.hierarchy = source.hierarchy;
      input.sources.push_back(std::move(source_ref));
    }

    return ValidateEmissionsSemantics(input);
  }

  Errors Validate(const Mechanism& mechanism)
  {
    Errors errors = ValidateGasModel(mechanism);

    Errors aerosol_errors = ValidateAerosolModel(mechanism);
    errors.insert(errors.end(), aerosol_errors.begin(), aerosol_errors.end());

    Errors emissions_errors = ValidateEmissionsModel(mechanism);
    errors.insert(errors.end(), emissions_errors.begin(), emissions_errors.end());

    return errors;
  }

}  // namespace mechanism_configuration

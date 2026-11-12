// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/error_format.hpp"
#include "detail/json_pointer.hpp"
#include "detail/semantics/aerosol.hpp"
#include "detail/semantics/emissions.hpp"
#include "detail/semantics/reactions.hpp"

#include <mechanism_configuration/validate.hpp>

#include <iomanip>
#include <map>
#include <optional>
#include <sstream>
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
        return ToString(*location) + " error: " + body;
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
          errors.push_back(
              { code,
                Message(ref.location, "Duplicate " + std::string(what) + " name '" + ref.name + "' found."),
                ref.path });
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
      phase_names.push_back({ phase.name, phase.location, phase.path });
      auto& registered = phase_species[phase.name];
      for (const auto& ps : phase.species)
      {
        registered.insert(ps.name);
        if (!species_names.contains(ps.name))
          errors.push_back(
              { ErrorCode::PhaseRequiresUnknownSpecies,
                Message(ps.location, "Unknown species name '" + ps.name + "' found in '" + phase.name + "' phase."),
                ps.path });
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
        errors.push_back(
            { ErrorCode::UnknownPhase,
              Message(reaction.location, "Unknown phase '" + reaction.phase + "' in '" + reaction.type + "' reaction."),
              reaction.path });

      for (const auto* components : { &reaction.reactants, &reaction.products })
        for (const auto& component : *components)
        {
          if (!species_names.contains(component.name))
            errors.push_back(
                { ErrorCode::ReactionRequiresUnknownSpecies,
                  Message(
                      component.location,
                      "Unknown species '" + component.name + "' used in '" + reaction.type + "' reaction."),
                  component.path });
          else if (phase_exists && !phase_it->second.contains(component.name))
            errors.push_back(
                { ErrorCode::RequestedSpeciesNotRegisteredInPhase,
                  Message(
                      component.location,
                      "Species '" + component.name + "' used in '" + reaction.type + "' is not defined in the '" +
                          reaction.phase + "' phase."),
                  component.path });
        }
      // The struct keeps the reactants of a photolysis in a list, so the list can have more than one.
      if (reaction.type == "PHOTOLYSIS" && reaction.reactants.size() > 1)
      {
        const auto& extra = reaction.reactants[1];
        errors.push_back(
            { ErrorCode::TooManyReactionComponents,
              Message(
                  extra.location,
                  "'" + reaction.type + "' reaction requires one reactant, but " +
                      std::to_string(reaction.reactants.size()) + " were provided."),
              extra.path });
      }
      for (const auto& species : reaction.needs_molecular_weight)
      {
        if (species_names.contains(species.name) && !input.species_with_molecular_weight.contains(species.name))
          errors.push_back(
              { ErrorCode::RequiredKeyNotFound,
                Message(
                    species.location,
                    "Species '" + species.name + "' used in '" + reaction.type + "' has no molecular weight defined."),
                species.path });
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
              Message(phase.location, "Unknown phase '" + phase.name + "' referenced by " + context + "."),
              phase.path });
        return nullptr;
      }
      const auto species_it = phase_it->second.find(species.name);
      if (species_it == phase_it->second.end())
      {
        errors.push_back(
            { ErrorCode::RequestedSpeciesNotRegisteredInPhase,
              Message(
                  species.location,
                  "Species '" + species.name + "' (" + context + ") is not defined in the '" + phase.name + "' phase."),
              species.path });
        return nullptr;
      }
      return species_it->second;
    };

    auto require_diffusion =
        [&](const semantics::NamedRef& phase, const semantics::NamedRef& species, const std::string& context)
    {
      const auto* entry = require_registered_species(phase, species, context);
      if (entry && !entry->has_diffusion_coefficient)
        errors.push_back(
            { ErrorCode::RequiredKeyNotFound,
              Message(
                  species.location,
                  context + ": species '" + species.name + "' has no diffusion coefficient defined in the '" + phase.name +
                      "' phase."),
              species.path });
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
                  context + ": species '" + species.name + "' has no density defined in the '" + phase.name + "' phase."),
              species.path });
    };

    auto require_molecular_weight = [&](const semantics::NamedRef& species, const std::string& context)
    {
      const auto species_it = species_index.find(species.name);
      if (species_it == species_index.end())
        errors.push_back(
            { ErrorCode::UnknownSpecies,
              Message(species.location, "Unknown species '" + species.name + "' referenced by " + context + "."),
              species.path });
      else if (!species_it->second->has_molecular_weight)
        errors.push_back(
            { ErrorCode::RequiredKeyNotFound,
              Message(species.location, context + ": species '" + species.name + "' has no molecular weight defined."),
              species.path });
    };

    // Verifies that phase exists.
    auto require_phase = [&](const semantics::NamedRef& phase, const std::string& context)
    {
      if (!phase_index.contains(phase.name))
        errors.push_back(
            { ErrorCode::UnknownPhase,
              Message(phase.location, "Unknown phase '" + phase.name + "' referenced by " + context + "."),
              phase.path });
    };

    for (const auto& representation : input.representations)
      for (const auto& phase : representation.phases)
        require_phase(phase, "aerosol representation '" + representation.name + "'");

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
      species_map_names.push_back({ smap.name, smap.location, smap.path });
    ReportDuplicates(species_map_names, ErrorCode::DuplicateSpeciesMapDetected, "species map", errors);

    std::vector<semantics::NamedRef> source_names;
    for (const auto& source : input.sources)
      source_names.push_back({ source.name, source.location, source.path });
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
        errors.push_back(
            { ErrorCode::SourceRequiresUnknownInventory,
              Message(
                  source.inventory.location,
                  "Source '" + source.name + "' references inventory '" + source.inventory.name +
                      "' which is not declared in 'inventories'."),
              source.inventory.path });

      if (!species_map_name_set.contains(source.species_map.name))
        errors.push_back(
            { ErrorCode::SourceRequiresUnknownSpeciesMap,
              Message(
                  source.species_map.location,
                  "Source '" + source.name + "' references species map '" + source.species_map.name +
                      "' which is not declared in 'species maps'."),
              source.species_map.path });

      if (cat_hier_counts[{ source.category, source.hierarchy }] > 1)
        errors.push_back(
            { ErrorCode::DuplicateCategoryHierarchy,
              Message(
                  source.location,
                  "Source '" + source.name + "' has duplicate (category: " + std::to_string(source.category) +
                      ", hierarchy: " + std::to_string(source.hierarchy) +
                      ") — each (category, hierarchy) pair must be unique."),
              source.path });
    }

    for (const auto& smap : input.species_maps)
    {
      std::unordered_map<std::string, double> scale_sum;
      for (const auto& mapping : smap.mappings)
        scale_sum[mapping.inventory_species] += mapping.scaling_factor;

      for (const auto& [inventory_species, total] : scale_sum)
      {
        if (total > 1.0 + 1e-9)
        {
          std::ostringstream message;
          message << "Species map '" << smap.name << "': scaling factors for inventory species '" << inventory_species
                  << "' sum to " << std::fixed << std::setprecision(4) << total << ", which exceeds 1.0.";
          errors.push_back({ ErrorCode::SpeciesMapScalingExceedsOne, Message(smap.location, message.str()), smap.path });
        }
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

    // Builds the component references of a list in a struct, with a path to each component,
    // for example "/reactions/arrhenius/0/products/1".
    std::vector<semantics::NamedRef> Refs(const std::vector<types::ReactionComponent>& components, const std::string& path)
    {
      std::vector<semantics::NamedRef> refs;
      refs.reserve(components.size());
      for (std::size_t i = 0; i < components.size(); ++i)
        refs.push_back({ components[i].name, std::nullopt, ChildPath(path, i) });
      return refs;
    }

    // Builds the reference of a single component in a struct, for example the gas-phase species
    // of a surface reaction.
    std::vector<semantics::NamedRef> Refs(const types::ReactionComponent& component, const std::string& path)
    {
      return { { component.name, std::nullopt, path } };
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

    // A Mechanism in code has no document. Its paths point into the struct, for example
    // "/reactions/arrhenius/1/products/0" for mechanism.reactions.arrhenius[1].products[0].
    const std::string species_path = RootPath("species");
    for (std::size_t i = 0; i < mechanism.species.size(); ++i)
    {
      const auto& s = mechanism.species[i];
      input.species.push_back({ s.name, std::nullopt, ChildPath(species_path, i) });
      if (s.molecular_weight)
        input.species_with_molecular_weight.insert(s.name);
    }

    const std::string phases_path = RootPath("phases");
    for (std::size_t i = 0; i < mechanism.phases.size(); ++i)
    {
      const auto& phase = mechanism.phases[i];
      const std::string phase_path = ChildPath(phases_path, i);
      semantics::PhaseRef pr{ phase.name, {}, std::nullopt, phase_path };
      for (std::size_t j = 0; j < phase.species.size(); ++j)
        pr.species.push_back({ phase.species[j].name, std::nullopt, ChildPath(phase_path, "species", j) });
      input.phases.push_back(std::move(pr));
    }

    const auto& r = mechanism.reactions;
    const std::string reactions_path = RootPath("reactions");
    // Adds the reactions of one type. `refs` gives the reactants and the products of a
    // reaction, from the reaction and its path.
    auto add_all = [&](std::string_view type, std::string_view field, const auto& reactions, auto refs)
    {
      const std::string type_path = ChildPath(reactions_path, field);
      for (std::size_t i = 0; i < reactions.size(); ++i)
      {
        const std::string path = ChildPath(type_path, i);
        auto [reactants, products] = refs(reactions[i], path);
        auto ref = ReactionRefOf(type, reactions[i].gas_phase, std::move(reactants), std::move(products));
        ref.path = ChildPath(path, "gas_phase");
        input.reactions.push_back(std::move(ref));
      }
    };
    // Most types have reactants and products. A first-order loss has one reactant.
    auto lists = [](const auto& x, const std::string& path)
    { return std::pair{ Refs(x.reactants, ChildPath(path, "reactants")), Refs(x.products, ChildPath(path, "products")) }; };

    add_all("ARRHENIUS", "arrhenius", r.arrhenius, lists);
    add_all("TROE", "troe", r.troe, lists);
    add_all("TERNARY_CHEMICAL_ACTIVATION", "ternary_chemical_activation", r.ternary_chemical_activation, lists);
    add_all(
        "TERNARY_CHEMICAL_ACTIVATION_JPL19",
        "ternary_chemical_activation_jpl19",
        r.ternary_chemical_activation_jpl19,
        lists);
    add_all("TUNNELING", "tunneling", r.tunneling, lists);
    add_all("TAYLOR_SERIES", "taylor_series", r.taylor_series, lists);
    add_all("USER_DEFINED", "user_defined", r.user_defined, lists);
    add_all("LAMBDA_RATE_CONSTANT", "lambda_rate_constant", r.lambda_rate_constant, lists);
    add_all(
        "EMISSION",
        "emission",
        r.emission,
        [](const auto& x, const std::string& path)
        { return std::pair{ std::vector<semantics::NamedRef>{}, Refs(x.products, ChildPath(path, "products")) }; });
    add_all("FIRST_ORDER_LOSS", "first_order_loss", r.first_order_loss, lists);
    add_all("PHOTOLYSIS", "photolysis", r.photolysis, lists);
    const std::size_t first_surface = input.reactions.size();
    add_all(
        "SURFACE",
        "surface",
        r.surface,
        [](const auto& x, const std::string& path)
        {
          return std::pair{ Refs(x.gas_phase_species, ChildPath(path, "gas_phase_species")),
                            Refs(x.gas_phase_products, ChildPath(path, "gas_phase_products")) };
        });
    // The gas-phase species of a surface reaction needs a molecular weight.
    for (std::size_t i = first_surface; i < input.reactions.size(); ++i)
      input.reactions[i].needs_molecular_weight = input.reactions[i].reactants;
    add_all(
        "BRANCHED",
        "branched",
        r.branched,
        [](const auto& x, const std::string& path)
        {
          // The products of both branches must be in the phase, and each keeps the path of its own list.
          auto products = Refs(x.alkoxy_products, ChildPath(path, "alkoxy_products"));
          auto nitrate = Refs(x.nitrate_products, ChildPath(path, "nitrate_products"));
          products.insert(products.end(), nitrate.begin(), nitrate.end());
          return std::pair{ Refs(x.reactants, ChildPath(path, "reactants")), std::move(products) };
        });

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

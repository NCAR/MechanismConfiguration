// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/error_format.hpp"

#include <mechanism_configuration/validate.hpp>

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
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

    // ---- Emissions ----------------------------------------------------------------------------
    if (input.emissions)
    {
      const auto& emissions = *input.emissions;

      ReportDuplicates(emissions.inventories, ErrorCode::DuplicateInventoryDetected, "inventory", errors);

      std::vector<semantics::NamedRef> species_map_names;
      for (const auto& smap : emissions.species_maps)
        species_map_names.push_back({ smap.name, smap.location });
      ReportDuplicates(species_map_names, ErrorCode::DuplicateSpeciesMapDetected, "species map", errors);

      std::vector<semantics::NamedRef> source_names;
      for (const auto& source : emissions.sources)
        source_names.push_back({ source.name, source.location });
      ReportDuplicates(source_names, ErrorCode::DuplicateSourceDetected, "source", errors);

      std::unordered_set<std::string> inventory_names;
      for (const auto& inv : emissions.inventories)
        inventory_names.insert(inv.name);

      std::unordered_set<std::string> species_map_name_set;
      for (const auto& smap : emissions.species_maps)
        species_map_name_set.insert(smap.name);

      std::map<std::pair<int, int>, int> cat_hier_counts;
      for (const auto& source : emissions.sources)
        ++cat_hier_counts[{ source.category, source.hierarchy }];

      for (const auto& source : emissions.sources)
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

      for (const auto& smap : emissions.species_maps)
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

    if (mechanism.emissions)
    {
      semantics::EmissionsRef emissions_ref;

      for (const auto& inv : mechanism.emissions->inventories)
        emissions_ref.inventories.push_back({ inv.name, std::nullopt });

      for (const auto& smap : mechanism.emissions->species_maps)
      {
        semantics::SpeciesMapRef smap_ref;
        smap_ref.name = smap.name;
        for (const auto& mapping : smap.mappings)
          smap_ref.mappings.push_back({ mapping.inventory_species, mapping.mechanism_species, mapping.scaling_factor });
        emissions_ref.species_maps.push_back(std::move(smap_ref));
      }

      for (const auto& source : mechanism.emissions->sources)
      {
        semantics::SourceRef source_ref;
        source_ref.name = source.name;
        source_ref.inventory = { source.inventory, std::nullopt };
        source_ref.species_map = { source.species_map, std::nullopt };
        source_ref.category = source.category;
        source_ref.hierarchy = source.hierarchy;
        emissions_ref.sources.push_back(std::move(source_ref));
      }

      input.emissions = std::move(emissions_ref);
    }

    return ValidateSemantics(input);
  }
}  // namespace mechanism_configuration

// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/format_compat.hpp>
#include <mechanism_configuration/validate.hpp>

#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mechanism_configuration
{
  namespace
  {
    // Emits one DuplicateSpeciesDetected/DuplicatePhasesDetected-style error per occurrence
    // of each name that appears more than once.
    void ReportDuplicates(
        const std::vector<std::string>& names,
        ErrorCode code,
        std::string_view what,
        Errors& errors)
    {
      std::unordered_map<std::string, int> counts;
      for (const auto& n : names)
        ++counts[n];
      for (const auto& n : names)
      {
        int total = counts[n];
        if (total > 1)
          errors.push_back({ code, mc_fmt::format("Duplicate {} name '{}' found.", what, n) });
      }
    }
  }  // namespace

  Errors validate(const Mechanism& mechanism)
  {
    Errors errors;

    // ---- Species ----------------------------------------------------------------------------
    std::unordered_set<std::string> species_names;
    {
      std::vector<std::string> names;
      names.reserve(mechanism.species.size());
      for (const auto& species : mechanism.species)
      {
        names.push_back(species.name);
        species_names.insert(species.name);
      }
      ReportDuplicates(names, ErrorCode::DuplicateSpeciesDetected, "species", errors);
    }

    // ---- Phases -----------------------------------------------------------------------------
    std::unordered_map<std::string, std::unordered_set<std::string>> phase_species;
    {
      std::vector<std::string> phase_names;
      for (const auto& phase : mechanism.phases)
      {
        phase_names.push_back(phase.name);
        auto& registered = phase_species[phase.name];

        std::vector<std::string> within;
        for (const auto& ps : phase.species)
        {
          within.push_back(ps.name);
          registered.insert(ps.name);
          if (!species_names.contains(ps.name))
            errors.push_back({ ErrorCode::PhaseRequiresUnknownSpecies,
                               mc_fmt::format("Unknown species name '{}' found in '{}' phase.", ps.name, phase.name) });
        }
        ReportDuplicates(within, ErrorCode::DuplicateSpeciesInPhaseDetected, "species", errors);
      }
      ReportDuplicates(phase_names, ErrorCode::DuplicatePhasesDetected, "phase", errors);
    }

    // ---- Reactions --------------------------------------------------------------------------
    // Reactants must exist and be registered in the reaction's phase; products must exist but
    // may reference species from any phase.
    auto check_reaction = [&](std::string_view type,
                              const std::string& gas_phase,
                              const std::vector<types::ReactionComponent>& reactants,
                              const std::vector<types::ReactionComponent>& products)
    {
      const auto phase_it = phase_species.find(gas_phase);
      const bool phase_exists = phase_it != phase_species.end();
      if (!phase_exists)
        errors.push_back(
            { ErrorCode::UnknownPhase, mc_fmt::format("Unknown phase '{}' in '{}' reaction.", gas_phase, type) });

      for (const auto& reactant : reactants)
      {
        if (!species_names.contains(reactant.name))
          errors.push_back({ ErrorCode::ReactionRequiresUnknownSpecies,
                             mc_fmt::format("Unknown species '{}' used in '{}' reaction.", reactant.name, type) });
        else if (phase_exists && !phase_it->second.contains(reactant.name))
          errors.push_back({ ErrorCode::RequestedSpeciesNotRegisteredInPhase,
                             mc_fmt::format("Species '{}' used in '{}' is not defined in the '{}' phase.",
                                            reactant.name, type, gas_phase) });
      }
      for (const auto& product : products)
      {
        if (!species_names.contains(product.name))
          errors.push_back({ ErrorCode::ReactionRequiresUnknownSpecies,
                             mc_fmt::format("Unknown species '{}' used in '{}' reaction.", product.name, type) });
      }
    };

    const auto& r = mechanism.reactions;
    for (const auto& x : r.arrhenius)
      check_reaction("ARRHENIUS", x.gas_phase, x.reactants, x.products);
    for (const auto& x : r.troe)
      check_reaction("TROE", x.gas_phase, x.reactants, x.products);
    for (const auto& x : r.ternary_chemical_activation)
      check_reaction("TERNARY_CHEMICAL_ACTIVATION", x.gas_phase, x.reactants, x.products);
    for (const auto& x : r.tunneling)
      check_reaction("TUNNELING", x.gas_phase, x.reactants, x.products);
    for (const auto& x : r.taylor_series)
      check_reaction("TAYLOR_SERIES", x.gas_phase, x.reactants, x.products);
    for (const auto& x : r.user_defined)
      check_reaction("USER_DEFINED", x.gas_phase, x.reactants, x.products);
    for (const auto& x : r.lambda_rate_constant)
      check_reaction("LAMBDA_RATE_CONSTANT", x.gas_phase, x.reactants, x.products);
    for (const auto& x : r.emission)
      check_reaction("EMISSION", x.gas_phase, {}, x.products);
    for (const auto& x : r.first_order_loss)
      check_reaction("FIRST_ORDER_LOSS", x.gas_phase, { x.reactants }, x.products);
    for (const auto& x : r.photolysis)
      check_reaction("PHOTOLYSIS", x.gas_phase, { x.reactants }, x.products);
    for (const auto& x : r.surface)
      check_reaction("SURFACE", x.gas_phase, { x.gas_phase_species }, x.gas_phase_products);
    for (const auto& x : r.branched)
    {
      std::vector<types::ReactionComponent> products = x.alkoxy_products;
      products.insert(products.end(), x.nitrate_products.begin(), x.nitrate_products.end());
      check_reaction("BRANCHED", x.gas_phase, x.reactants, products);
    }

    return errors;
  }
}  // namespace mechanism_configuration

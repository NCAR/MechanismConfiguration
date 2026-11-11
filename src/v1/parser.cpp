// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/parser.hpp"

#include "detail/error_format.hpp"
#include "detail/json_pointer.hpp"
#include "detail/schema.hpp"
#include "detail/v1/aerosol/keys.hpp"
#include "detail/v1/aerosol/parsers.hpp"
#include "detail/v1/aerosol/schema.hpp"
#include "detail/v1/emissions/keys.hpp"
#include "detail/v1/emissions/parsers.hpp"
#include "detail/v1/emissions/schema.hpp"
#include "detail/v1/keys.hpp"
#include "detail/v1/reactions/parsers.hpp"
#include "detail/v1/reactions/schema.hpp"
#include "detail/v1/species/keys.hpp"
#include "detail/v1/species/parsers.hpp"
#include "detail/v1/species/schema.hpp"
#include "detail/v1/utils.hpp"

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/validate.hpp>

#include <yaml-cpp/yaml.h>

#include <array>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace mechanism_configuration::v1
{
  namespace
  {
    // How a top-level section (species / phases / reactions) is expressed.
    enum class EntityFormat
    {
      FileList,  // { "files": [...] } — split across other files (v1.1+)
      Inline,    // [ ... ] — listed directly
      Invalid,   // present but neither of the above
    };

    EntityFormat GetEntityFormat(const YAML::Node& node)
    {
      if (node.IsMap() && node["files"])
        return EntityFormat::FileList;
      if (node.IsSequence())
        return EntityFormat::Inline;
      return EntityFormat::Invalid;
    }

    // Appends each component under `key` (reactant- or product-like) as a located reference.
    // `path` is the JSON Pointer to `reaction`.
    void CollectComponents(
        const YAML::Node& reaction,
        const std::string& path,
        std::string_view key,
        std::vector<semantics::NamedRef>& out)
    {
      const std::string k(key);
      if (!reaction[k])
        return;
      const std::string list_path = ChildPath(path, key);
      std::size_t index = 0;
      for (const auto& item : AsSequence(reaction[k]))
        out.push_back({ GetComponentName(item), LocationOf(item), ItemPath(reaction[k], list_path, index++) });
    }

    // The newest v1 minor version that this library can parse.
    constexpr unsigned int NEWEST_MINOR_VERSION = 4;

    // The first v1 minor version that allows the file-list format.
    constexpr unsigned int FILE_LIST_MINIMUM_MINOR_VERSION = 1;

    // The first v1 minor version that allows each optional top-level section.
    constexpr std::array<std::pair<std::string_view, unsigned int>, 3> SECTION_MINIMUM_MINOR_VERSION = { {
        { keys::aerosol_representations, 2 },
        { keys::aerosol_processes, 2 },
        { keys::emissions, 3 },
    } };

    // The first v1 minor version that allows each reaction type that is newer than 1.0.
    constexpr std::array<std::pair<std::string_view, unsigned int>, 1> REACTION_TYPE_MINIMUM_MINOR_VERSION = { {
        { keys::TernaryChemicalActivationJPL19_key, 4 },
    } };

    // A feature that the configuration uses, and the first v1 minor version that allows it.
    struct VersionRequirement
    {
      std::string feature;  // the feature name in the error message
      YAML::Node node;      // the node that the error points to
      std::string path;     // the JSON Pointer to `node`
      unsigned int minimum_minor;
    };

    // Checks that the declared minor version is supported, and that it is new enough for
    // the file-list format, for each optional section, and for each reaction type that the configuration uses.
    // Only the unmerged configuration shows the file-list format, so the caller gives it.
    Errors CheckMinorVersion(const YAML::Node& object, const Version& version, bool uses_file_list)
    {
      Errors errors;
      const YAML::Node version_node = object[std::string(keys::version)];

      if (version.minor > NEWEST_MINOR_VERSION)
      {
        errors.push_back({ ErrorCode::InvalidVersion,
                           ToString(LocationOf(version_node)) + " error: Version '" + version.to_string() +
                               "' is not supported. The newest supported version is '1." +
                               std::to_string(NEWEST_MINOR_VERSION) + "'.",
                           RootPath(keys::version) });
        return errors;
      }

      std::vector<VersionRequirement> requirements;
      if (uses_file_list)
        requirements.push_back(
            { "The file-list format", version_node, RootPath(keys::version), FILE_LIST_MINIMUM_MINOR_VERSION });

      for (const auto& [section, minimum_minor] : SECTION_MINIMUM_MINOR_VERSION)
      {
        const YAML::Node node = object[std::string(section)];
        if (node)
          requirements.push_back({ "'" + std::string(section) + "'", node, RootPath(section), minimum_minor });
      }

      // Reactions in a file list are not loaded yet; the merged configuration is checked again later.
      const YAML::Node reactions = object[std::string(keys::reactions)];
      if (reactions && reactions.IsSequence())
      {
        std::size_t index = 0;
        for (const auto& reaction : reactions)
        {
          const std::string path = RootPath(keys::reactions, index++);
          if (!reaction.IsMap() || !reaction[std::string(keys::type)] || !reaction[std::string(keys::type)].IsScalar())
            continue;
          const std::string type = reaction[std::string(keys::type)].as<std::string>();
          for (const auto& [reaction_type, minimum_minor] : REACTION_TYPE_MINIMUM_MINOR_VERSION)
            if (type == reaction_type)
              requirements.push_back({ "The '" + type + "' reaction type",
                                       reaction[std::string(keys::type)],
                                       ChildPath(path, keys::type),
                                       minimum_minor });
        }
      }

      for (const auto& requirement : requirements)
      {
        if (version.minor < requirement.minimum_minor)
          errors.push_back({ ErrorCode::InvalidVersion,
                             ToString(LocationOf(requirement.node)) + " error: " + requirement.feature +
                                 " requires version '1." + std::to_string(requirement.minimum_minor) +
                                 "' or newer, but the version is '" + version.to_string() + "'.",
                             requirement.path });
      }
      return errors;
    }
    // Tells if the parser left out the item at `path`, so that the checks skip it.
    bool IsLeftOut(const LeftOutItems& left_out, const std::string& path)
    {
      return left_out.paths.contains(path);
    }
  }  // namespace

  semantics::ReactionsInput BuildReactionsSemanticInput(const YAML::Node& object, const LeftOutItems& left_out)
  {
    semantics::ReactionsInput input;

    if (object[std::string(keys::species)])
    {
      std::size_t index = 0;
      for (const auto& s : object[std::string(keys::species)])
      {
        const std::string path = RootPath(keys::species, index++);
        if (IsLeftOut(left_out, path))
          continue;
        input.species.push_back({ GetComponentName(s), LocationOf(s), path });
        if (s[std::string(keys::molecular_weight)])
          input.species_with_molecular_weight.insert(GetComponentName(s));
      }
    }

    if (object[std::string(keys::phases)])
    {
      std::size_t index = 0;
      for (const auto& phase : object[std::string(keys::phases)])
      {
        const std::string path = RootPath(keys::phases, index++);
        if (IsLeftOut(left_out, path))
          continue;
        semantics::PhaseRef pr;
        pr.name = phase[std::string(keys::name)].as<std::string>();
        pr.location = LocationOf(phase);
        pr.path = path;
        if (phase[std::string(keys::species)])
        {
          const std::string species_path = ChildPath(*pr.path, keys::species);
          std::size_t species_index = 0;
          for (const auto& ps : phase[std::string(keys::species)])
            pr.species.push_back({ GetComponentName(ps), LocationOf(ps), ChildPath(species_path, species_index++) });
        }
        input.phases.push_back(std::move(pr));
      }
    }

    if (object[std::string(keys::reactions)])
    {
      std::size_t index = 0;
      for (const auto& reaction : object[std::string(keys::reactions)])
      {
        const std::string path = RootPath(keys::reactions, index++);
        if (IsLeftOut(left_out, path))
          continue;
        semantics::ReactionRef rr;
        if (reaction[std::string(keys::type)])
          rr.type = reaction[std::string(keys::type)].as<std::string>();
        if (reaction[std::string(keys::gas_phase)])
        {
          rr.phase = reaction[std::string(keys::gas_phase)].as<std::string>();
          rr.location = LocationOf(reaction[std::string(keys::gas_phase)]);
          rr.path = ChildPath(path, keys::gas_phase);
        }
        CollectComponents(reaction, path, keys::reactants, rr.reactants);
        CollectComponents(reaction, path, keys::gas_phase_species, rr.reactants);
        CollectComponents(reaction, path, keys::gas_phase_species, rr.needs_molecular_weight);
        CollectComponents(reaction, path, keys::products, rr.products);
        CollectComponents(reaction, path, keys::alkoxy_products, rr.products);
        CollectComponents(reaction, path, keys::nitrate_products, rr.products);
        CollectComponents(reaction, path, keys::gas_phase_products, rr.products);
        input.reactions.push_back(std::move(rr));
      }
    }

    return input;
  }

  semantics::AerosolInput BuildAerosolSemanticInput(const YAML::Node& object, const LeftOutItems& left_out)
  {
    semantics::AerosolInput input;

    std::size_t species_index = 0;
    if (object[std::string(keys::species)])
      for (const auto& s : object[std::string(keys::species)])
      {
        if (IsLeftOut(left_out, RootPath(keys::species, species_index++)))
          continue;
        input.species.push_back({ GetComponentName(s), static_cast<bool>(s[std::string(keys::molecular_weight)]) });
      }

    std::size_t phase_index = 0;
    if (object[std::string(keys::phases)])
      for (const auto& phase : object[std::string(keys::phases)])
      {
        if (IsLeftOut(left_out, RootPath(keys::phases, phase_index++)))
          continue;
        semantics::PhaseDef phase_def;
        phase_def.name = phase[std::string(keys::name)].as<std::string>();
        if (phase[std::string(keys::species)])
          for (const auto& ps : phase[std::string(keys::species)])
          {
            semantics::PhaseSpeciesDef ps_def;
            ps_def.name = GetComponentName(ps);
            if (!ps.IsScalar())
            {
              ps_def.has_diffusion_coefficient = static_cast<bool>(ps[std::string(keys::diffusion_coefficient)]);
              ps_def.has_density = static_cast<bool>(ps[std::string(keys::density)]);
            }
            phase_def.species.push_back(std::move(ps_def));
          }
        input.phases.push_back(std::move(phase_def));
      }

    // Sets mechanism.aerosol when both keys are present, avoiding errors for references omitted from the built Mechanism.
    if (!(object[std::string(keys::aerosol_representations)] && object[std::string(keys::aerosol_processes)]) ||
        IsLeftOut(left_out, RootPath(keys::aerosol_representations)))
      return input;

    std::size_t representation_index = 0;
    for (const auto& rep_node : object[std::string(keys::aerosol_representations)])
    {
      const std::string rep_path = RootPath(keys::aerosol_representations, representation_index++);
      semantics::AerosolRepresentationRef ref;
      ref.name = rep_node[std::string(keys::name)].as<std::string>();
      ref.location = LocationOf(rep_node);
      if (rep_node[std::string(keys::phases)])
      {
        const std::string phases_path = ChildPath(rep_path, keys::phases);
        std::size_t phase_index = 0;
        for (const auto& phase_node : rep_node[std::string(keys::phases)])
          ref.phases.push_back(
              { phase_node.as<std::string>(), LocationOf(phase_node), ChildPath(phases_path, phase_index++) });
      }
      input.representations.push_back(std::move(ref));
    }

    std::size_t process_index = 0;
    for (const auto& entry : object[std::string(keys::aerosol_processes)])
    {
      const std::string entry_path = RootPath(keys::aerosol_processes, process_index++);
      const auto type = entry[std::string(keys::type)].as<std::string>();

      if (type == keys::HenrysLawPhaseTransfer_key)
      {
        semantics::HenrysLawPhaseTransferRef ref;
        ref.gas_phase = { entry[std::string(keys::gas_phase)].as<std::string>(),
                          LocationOf(entry[std::string(keys::gas_phase)]),
                          ChildPath(entry_path, keys::gas_phase) };
        ref.gas_species = { entry[std::string(keys::gas_phase_species)].as<std::string>(),
                            LocationOf(entry[std::string(keys::gas_phase_species)]),
                            ChildPath(entry_path, keys::gas_phase_species) };
        ref.condensed_phase = { entry[std::string(keys::condensed_phase)].as<std::string>(),
                                LocationOf(entry[std::string(keys::condensed_phase)]),
                                ChildPath(entry_path, keys::condensed_phase) };
        ref.condensed_species = { entry[std::string(keys::condensed_phase_species)].as<std::string>(),
                                  LocationOf(entry[std::string(keys::condensed_phase_species)]),
                                  ChildPath(entry_path, keys::condensed_phase_species) };
        ref.solvent = { entry[std::string(keys::solvent)].as<std::string>(),
                        LocationOf(entry[std::string(keys::solvent)]),
                        ChildPath(entry_path, keys::solvent) };
        ref.location = LocationOf(entry);
        input.henrys_law_phase_transfers.push_back(std::move(ref));
      }
      else if (type == keys::DissolvedReaction_key)
      {
        semantics::DissolvedReactionRef ref;
        ref.phase = { entry[std::string(keys::condensed_phase)].as<std::string>(),
                      LocationOf(entry[std::string(keys::condensed_phase)]),
                      ChildPath(entry_path, keys::condensed_phase) };
        ref.solvent = { entry[std::string(keys::solvent)].as<std::string>(),
                        LocationOf(entry[std::string(keys::solvent)]),
                        ChildPath(entry_path, keys::solvent) };
        CollectComponents(entry, entry_path, keys::reactants, ref.reactants);
        CollectComponents(entry, entry_path, keys::products, ref.products);
        ref.location = LocationOf(entry);
        input.dissolved_reactions.push_back(std::move(ref));
      }
      else if (type == keys::DissolvedReversibleReaction_key)
      {
        semantics::DissolvedReversibleReactionRef ref;
        ref.phase = { entry[std::string(keys::condensed_phase)].as<std::string>(),
                      LocationOf(entry[std::string(keys::condensed_phase)]),
                      ChildPath(entry_path, keys::condensed_phase) };
        ref.solvent = { entry[std::string(keys::solvent)].as<std::string>(),
                        LocationOf(entry[std::string(keys::solvent)]),
                        ChildPath(entry_path, keys::solvent) };
        CollectComponents(entry, entry_path, keys::reactants, ref.reactants);
        CollectComponents(entry, entry_path, keys::products, ref.products);
        ref.location = LocationOf(entry);
        input.dissolved_reversible_reactions.push_back(std::move(ref));
      }
      else if (type == keys::HenrysLawEquilibrium_key)
      {
        semantics::HenrysLawEquilibriumRef ref;
        ref.gas_phase = { entry[std::string(keys::gas_phase)].as<std::string>(),
                          LocationOf(entry[std::string(keys::gas_phase)]),
                          ChildPath(entry_path, keys::gas_phase) };
        ref.gas_species = { entry[std::string(keys::gas_phase_species)].as<std::string>(),
                            LocationOf(entry[std::string(keys::gas_phase_species)]),
                            ChildPath(entry_path, keys::gas_phase_species) };
        ref.condensed_phase = { entry[std::string(keys::condensed_phase)].as<std::string>(),
                                LocationOf(entry[std::string(keys::condensed_phase)]),
                                ChildPath(entry_path, keys::condensed_phase) };
        ref.condensed_species = { entry[std::string(keys::condensed_phase_species)].as<std::string>(),
                                  LocationOf(entry[std::string(keys::condensed_phase_species)]),
                                  ChildPath(entry_path, keys::condensed_phase_species) };
        ref.solvent = { entry[std::string(keys::solvent)].as<std::string>(),
                        LocationOf(entry[std::string(keys::solvent)]),
                        ChildPath(entry_path, keys::solvent) };
        ref.location = LocationOf(entry);
        input.henrys_law_equilibria.push_back(std::move(ref));
      }
      else if (type == keys::DissolvedEquilibrium_key)
      {
        semantics::DissolvedEquilibriumRef ref;
        ref.phase = { entry[std::string(keys::condensed_phase)].as<std::string>(),
                      LocationOf(entry[std::string(keys::condensed_phase)]),
                      ChildPath(entry_path, keys::condensed_phase) };
        ref.algebraic_species = { entry[std::string(keys::algebraic_species)].as<std::string>(),
                                  LocationOf(entry[std::string(keys::algebraic_species)]),
                                  ChildPath(entry_path, keys::algebraic_species) };
        ref.solvent = { entry[std::string(keys::solvent)].as<std::string>(),
                        LocationOf(entry[std::string(keys::solvent)]),
                        ChildPath(entry_path, keys::solvent) };
        CollectComponents(entry, entry_path, keys::reactants, ref.reactants);
        CollectComponents(entry, entry_path, keys::products, ref.products);
        ref.location = LocationOf(entry);
        input.dissolved_equilibria.push_back(std::move(ref));
      }
      else if (type == keys::LinearConstraint_key)
      {
        semantics::LinearConstraintRef ref;
        ref.algebraic_phase = { entry[std::string(keys::algebraic_phase)].as<std::string>(),
                                LocationOf(entry[std::string(keys::algebraic_phase)]),
                                ChildPath(entry_path, keys::algebraic_phase) };
        ref.algebraic_species = { entry[std::string(keys::algebraic_species)].as<std::string>(),
                                  LocationOf(entry[std::string(keys::algebraic_species)]),
                                  ChildPath(entry_path, keys::algebraic_species) };
        if (entry[std::string(keys::terms)])
        {
          const std::string terms_path = ChildPath(entry_path, keys::terms);
          std::size_t term_index = 0;
          for (const auto& term_node : entry[std::string(keys::terms)])
          {
            const std::string term_path = ChildPath(terms_path, term_index++);
            semantics::LinearConstraintTermRef term_ref;
            term_ref.phase = { term_node[std::string(keys::phase)].as<std::string>(),
                               LocationOf(term_node[std::string(keys::phase)]),
                               ChildPath(term_path, keys::phase) };
            term_ref.species = { term_node[std::string(keys::name)].as<std::string>(),
                                 LocationOf(term_node[std::string(keys::name)]),
                                 ChildPath(term_path, keys::name) };
            ref.terms.push_back(std::move(term_ref));
          }
        }
        ref.location = LocationOf(entry);
        input.linear_constraints.push_back(std::move(ref));
      }
    }

    return input;
  }

  semantics::EmissionsInput BuildEmissionsSemanticInput(const YAML::Node& object, const LeftOutItems& left_out)
  {
    semantics::EmissionsInput input;

    if (!object[std::string(keys::emissions)] || IsLeftOut(left_out, RootPath(keys::emissions)))
      return input;

    const YAML::Node& emissions_node = object[std::string(keys::emissions)];
    const std::string emissions_path = RootPath(keys::emissions);

    if (emissions_node[std::string(keys::inventories)])
    {
      const std::string inventories_path = ChildPath(emissions_path, keys::inventories);
      std::size_t index = 0;
      for (const auto& item : emissions_node[std::string(keys::inventories)])
        input.inventories.push_back(
            { item[std::string(keys::name)].as<std::string>(), LocationOf(item), ChildPath(inventories_path, index++) });
    }

    if (emissions_node[std::string(keys::species_maps)])
    {
      const std::string species_maps_path = ChildPath(emissions_path, keys::species_maps);
      std::size_t index = 0;
      for (const auto& item : emissions_node[std::string(keys::species_maps)])
      {
        semantics::SpeciesMapRef smap_ref;
        smap_ref.name = item[std::string(keys::name)].as<std::string>();
        smap_ref.location = LocationOf(item);
        smap_ref.path = ChildPath(species_maps_path, index++);
        if (item[std::string(keys::mappings)])
          for (const auto& mapping_node : item[std::string(keys::mappings)])
          {
            semantics::SpeciesMappingRef mapping_ref;
            mapping_ref.inventory_species = mapping_node[std::string(keys::inventory_species)].as<std::string>();
            mapping_ref.mechanism_species = mapping_node[std::string(keys::mechanism_species)].as<std::string>();
            if (mapping_node[std::string(keys::scaling_factor)])
              mapping_ref.scaling_factor = mapping_node[std::string(keys::scaling_factor)].as<double>();
            smap_ref.mappings.push_back(std::move(mapping_ref));
          }
        input.species_maps.push_back(std::move(smap_ref));
      }
    }

    if (emissions_node[std::string(keys::sources)])
    {
      const std::string sources_path = ChildPath(emissions_path, keys::sources);
      std::size_t index = 0;
      for (const auto& item : emissions_node[std::string(keys::sources)])
      {
        const std::string source_path = ChildPath(sources_path, index++);
        semantics::SourceRef source_ref;
        source_ref.name = item[std::string(keys::name)].as<std::string>();
        source_ref.location = LocationOf(item);
        source_ref.path = source_path;
        source_ref.inventory = { item[std::string(keys::inventory)].as<std::string>(),
                                 LocationOf(item[std::string(keys::inventory)]),
                                 ChildPath(source_path, keys::inventory) };
        source_ref.species_map = { item[std::string(keys::species_map)].as<std::string>(),
                                   LocationOf(item[std::string(keys::species_map)]),
                                   ChildPath(source_path, keys::species_map) };
        if (item[std::string(keys::category)])
          source_ref.category = item[std::string(keys::category)].as<int>();
        if (item[std::string(keys::hierarchy)])
          source_ref.hierarchy = item[std::string(keys::hierarchy)].as<int>();
        input.sources.push_back(std::move(source_ref));
      }
    }

    return input;
  }

  std::expected<YAML::Node, Errors> Parser::ResolveFileConfig(const std::filesystem::path& config_path)
  {
    if (!std::filesystem::exists(config_path) || !std::filesystem::is_regular_file(config_path))
    {
      return std::unexpected(
          Errors{ { ErrorCode::FileNotFound,
                    "Configuration file '" + config_path.string() + "' does not exist or is not a regular file." } });
    }

    SetConfigPath(config_path.string());

    YAML::Node object;
    try
    {
      object = YAML::LoadFile(config_path.string());
    }
    catch (const std::exception& e)
    {
      return std::unexpected(Errors{
          { ErrorCode::UnexpectedError, "Failed to parse '" + config_path.string() + "': " + std::string(e.what()) } });
    }

    Errors errors;
    const std::filesystem::path base_dir = config_path.parent_path();
    const Version version = object[keys::version] ? Version(object[keys::version].as<std::string>()) : Version();

    YAML::Node combined;
    if (object[keys::version])
      combined[std::string(keys::version)] = object[keys::version];
    if (object[keys::name])
      combined[std::string(keys::name)] = object[keys::name];
    // Keep the unknown top-level properties (keys that start with __)
    for (const auto& item : object)
    {
      const std::string key = item.first.as<std::string>();
      if (key.starts_with("__"))
        combined[key] = item.second;
    }

    // Loads and concatenates every file referenced under `<entity>.files`.
    auto load_files = [&](std::string_view entity) -> YAML::Node
    {
      YAML::Node merged(YAML::NodeType::Sequence);
      const std::string files_path = RootPath(entity, "files");
      std::size_t index = 0;
      for (const auto& file_node : object[std::string(entity)]["files"])
      {
        const std::string file_node_path = ChildPath(files_path, index++);
        const std::filesystem::path file_path = base_dir / file_node.as<std::string>();
        if (!std::filesystem::exists(file_path))
        {
          errors.push_back({ ErrorCode::FileNotFound, "File not found: " + file_path.string(), file_node_path });
          continue;
        }
        try
        {
          YAML::Node loaded = YAML::LoadFile(file_path.string());
          for (const auto& item : loaded)
            merged.push_back(item);
        }
        catch (const std::exception& e)
        {
          errors.push_back({ ErrorCode::UnexpectedError,
                             "Failed to parse file: " + file_path.string() + ": " + e.what(),
                             file_node_path });
        }
      }
      return merged;
    };

    // Resolves one section into the combined node. Missing sections are left out so the
    // normal schema validation reports them; malformed sections are flagged here.
    auto resolve_section = [&](std::string_view entity)
    {
      const std::string key(entity);
      if (!object[key])
        return;

      switch (GetEntityFormat(object[key]))
      {
        case EntityFormat::Inline: combined[key] = object[key]; break;
        case EntityFormat::FileList: combined[key] = load_files(entity); break;
        case EntityFormat::Invalid:
          if (object[key].IsMap())
            errors.push_back(
                { ErrorCode::RequiredKeyNotFound, "Missing 'files' key in '" + key + "' section.", RootPath(key) });
          else
            errors.push_back(
                { ErrorCode::InvalidType, "'" + key + "' must be a file-list object or an inline array.", RootPath(key) });
          break;
      }
    };
    // Check the minor version before loading any files.
    const bool uses_filelist = (object[std::string(keys::species)] &&
                                GetEntityFormat(object[std::string(keys::species)]) == EntityFormat::FileList) ||
                               (object[std::string(keys::phases)] &&
                                GetEntityFormat(object[std::string(keys::phases)]) == EntityFormat::FileList) ||
                               (object[std::string(keys::reactions)] &&
                                GetEntityFormat(object[std::string(keys::reactions)]) == EntityFormat::FileList);
    if (version.major == 1)
    {
      auto version_errors = CheckMinorVersion(object, version, uses_filelist);
      if (!version_errors.empty())
      {
        AppendFilePath(config_path_, version_errors);
        return std::unexpected(std::move(version_errors));
      }
    }

    resolve_section(keys::species);
    resolve_section(keys::phases);
    resolve_section(keys::reactions);
    resolve_section(keys::aerosol_representations);
    resolve_section(keys::aerosol_processes);

    if (object[std::string(keys::emissions)])
      combined[std::string(keys::emissions)] = object[std::string(keys::emissions)];

    if (!errors.empty())
    {
      AppendFilePath(config_path_, errors);
      return std::unexpected(std::move(errors));
    }

    return combined;
  }

  Errors Parser::CheckSchema(const YAML::Node& object)
  {
    Errors errors;

    std::vector<std::string_view> required_keys = { keys::version, keys::species, keys::phases };
    std::vector<std::string_view> optional_keys = {
      keys::name, keys::reactions, keys::aerosol_representations, keys::aerosol_processes, keys::emissions
    };

    // Return early if the required keys are not found
    auto schema_errors = mechanism_configuration::CheckSchema(object, required_keys, optional_keys);
    if (!schema_errors.empty())
    {
      SetMissingPaths(schema_errors, "");
      AppendFilePath(config_path_, schema_errors);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
    }

    // A config must define at least one mechanism mode: gas-phase reactions, or the aerosol
    // pair 'aerosol representations' + 'aerosol processes'. The two modes may also coexist.
    const bool has_reactions = static_cast<bool>(object[keys::reactions]);
    const bool has_aerosol_representations = static_cast<bool>(object[keys::aerosol_representations]);
    const bool has_aerosol_processes = static_cast<bool>(object[keys::aerosol_processes]);
    Errors mode_errors;
    if (has_aerosol_representations != has_aerosol_processes)
    {
      ErrorLocation error_location{ object.Mark().line, object.Mark().column };
      mode_errors.push_back(
          { ErrorCode::RequiredKeyNotFound,
            ToString(error_location) + " error: '" + std::string(keys::aerosol_representations) + "' and '" +
                std::string(keys::aerosol_processes) + "' must be provided together.",
            "" });
    }
    if (!has_reactions && !(has_aerosol_representations && has_aerosol_processes))
    {
      ErrorLocation error_location{ object.Mark().line, object.Mark().column };
      mode_errors.push_back(
          { ErrorCode::RequiredKeyNotFound,
            ToString(error_location) + " error: A configuration must contain either '" + std::string(keys::reactions) +
                "' or both '" + std::string(keys::aerosol_representations) + "' and '" +
                std::string(keys::aerosol_processes) + "'.",
            "" });
    }
    if (!mode_errors.empty())
    {
      AppendFilePath(config_path_, mode_errors);
      errors.insert(errors.end(), mode_errors.begin(), mode_errors.end());
    }

    // The check of the required keys reports a missing version.
    if (object[keys::version])
    {
      constexpr unsigned int MAJOR_VERSION = 1;
      Version version = Version(object[keys::version].as<std::string>());
      if (version.major != MAJOR_VERSION)
      {
        ErrorLocation error_location{ object[keys::version].Mark().line, object[keys::version].Mark().column };

        std::string message = ToString(error_location) + " error: The version must be '" + std::to_string(MAJOR_VERSION) +
                              "' but the invalid version number '" + std::to_string(version.major) + "' found.";
        errors.push_back({ ErrorCode::InvalidVersion, config_path_ + ":" + message, RootPath(keys::version) });
      }
      else
      {
        // A file configuration is already merged here, so its file-list format was checked in ResolveFileConfig.
        auto version_errors = CheckMinorVersion(object, version, false);
        AppendFilePath(config_path_, version_errors);
        errors.insert(errors.end(), version_errors.begin(), version_errors.end());
      }
    }

    // Each section is checked, also when an earlier section has errors, so that the errors of
    // all sections are reported. The semantic checks of a full parse still run only on a
    // document without schema errors.
    if (object[keys::species])
    {
      schema_errors = CheckSpeciesSchema(object[keys::species], RootPath(keys::species));
      if (!schema_errors.empty())
      {
        AppendFilePath(config_path_, schema_errors);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }
    }

    if (object[keys::phases])
    {
      schema_errors = CheckPhasesSchema(object[keys::phases], RootPath(keys::phases));
      if (!schema_errors.empty())
      {
        AppendFilePath(config_path_, schema_errors);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }
    }

    // Gas-phase reactions are optional (an aerosol-only config may omit them).
    if (has_reactions)
    {
      schema_errors = CheckReactionsSchema(object[keys::reactions], RootPath(keys::reactions));
      if (!schema_errors.empty())
      {
        AppendFilePath(config_path_, schema_errors);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }
    }

    // Aerosol sections are optional.
    if (has_aerosol_representations && has_aerosol_processes)
    {
      schema_errors =
          CheckAerosolRepresentationsSchema(object[keys::aerosol_representations], RootPath(keys::aerosol_representations));
      if (!schema_errors.empty())
      {
        AppendFilePath(config_path_, schema_errors);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }

      schema_errors = CheckAerosolProcessesSchema(object[keys::aerosol_processes], RootPath(keys::aerosol_processes));
      if (!schema_errors.empty())
      {
        AppendFilePath(config_path_, schema_errors);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }
    }

    if (object[std::string(keys::emissions)])
    {
      schema_errors = CheckEmissionsSchema(object[std::string(keys::emissions)], RootPath(keys::emissions));
      if (!schema_errors.empty())
      {
        AppendFilePath(config_path_, schema_errors);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
        return errors;
      }
    }

    return errors;
  }

  ParseResult Parser::Parse(const std::filesystem::path& config_path)
  {
    // ResolveFileConfig sets config_path_ so errors carry the file path.
    auto object = ResolveFileConfig(config_path);
    if (!object)
    {
      return ParseResult{ std::nullopt, std::move(object.error()) };
    }
    return ValidateAndBuild(*object);
  }

  ParseResult Parser::Parse(const std::string& content)
  {
    SetDefaultConfigPath();  // no file backing this document
    YAML::Node object;
    try
    {
      object = YAML::Load(content);
    }
    catch (const std::exception& e)
    {
      return ParseResult{ std::nullopt,
                          { { ErrorCode::UnexpectedError, "Failed to parse document: " + std::string(e.what()) } } };
    }
    return ValidateAndBuild(object);
  }

  ParseResult Parser::Parse(const YAML::Node& object)
  {
    SetDefaultConfigPath();  // no file backing this node
    return ValidateAndBuild(object);
  }

  ParseResult Parser::ValidateAndBuild(const YAML::Node& object)
  {
    try
    {
      Errors errors = CheckSchema(object);

      // Builds every item that it can, also an item with a schema error. The semantic checks
      // then skip the items that the mechanism does not have.
      LeftOutItems left_out;
      Mechanism mechanism = Build(object, left_out);
      auto semantic_errors = ValidateReactionsSemantics(BuildReactionsSemanticInput(object, left_out));
      auto aerosol_errors = ValidateAerosolSemantics(BuildAerosolSemanticInput(object, left_out));
      auto emissions_errors = ValidateEmissionsSemantics(BuildEmissionsSemanticInput(object, left_out));
      semantic_errors.insert(semantic_errors.begin(), left_out.errors.begin(), left_out.errors.end());
      semantic_errors.insert(semantic_errors.end(), aerosol_errors.begin(), aerosol_errors.end());
      semantic_errors.insert(semantic_errors.end(), emissions_errors.begin(), emissions_errors.end());
      AppendFilePath(config_path_, semantic_errors);
      errors.insert(errors.end(), semantic_errors.begin(), semantic_errors.end());

      return ParseResult{ std::move(mechanism), std::move(errors) };
    }
    catch (const std::exception& e)
    {
      const std::string where = config_path_.empty() ? "document" : "'" + config_path_ + "'";
      return ParseResult{ std::nullopt,
                          { { ErrorCode::UnexpectedError, "Failed to parse " + where + ": " + std::string(e.what()) } } };
    }
  }

  Mechanism Parser::Build(const YAML::Node& object, LeftOutItems& left_out)
  {
    Mechanism mechanism;

    // A document with errors can have no version, species, or phases.
    if (object[keys::version])
      mechanism.version = Version(object[keys::version].as<std::string>());
    if (object[keys::species])
      mechanism.species = ParseSpecies(object[keys::species], left_out);
    if (object[keys::phases])
      mechanism.phases = ParsePhases(object[keys::phases], left_out);

    if (object[keys::name])
    {
      mechanism.name = object[keys::name].as<std::string>();
    }
    if (object[keys::reactions])
    {
      mechanism.reactions = ParseReactions(object[keys::reactions], left_out);
    }
    // The aerosol and emissions items refer to each other, so each section is built as a whole.
    if (object[keys::aerosol_representations] && object[keys::aerosol_processes])
    {
      BuildItem(
          object[keys::aerosol_representations],
          RootPath(keys::aerosol_representations),
          "aerosol section",
          left_out,
          [&] { mechanism.aerosol = ParseAerosol(object, mechanism.species, mechanism.phases); });
    }
    if (object[std::string(keys::emissions)])
    {
      BuildItem(
          object[std::string(keys::emissions)],
          RootPath(keys::emissions),
          "emissions section",
          left_out,
          [&] { mechanism.emissions = ParseEmissions(object[std::string(keys::emissions)]); });
    }
    mechanism.unknown_properties = GetComments(object);

    return mechanism;
  }

}  // namespace mechanism_configuration::v1

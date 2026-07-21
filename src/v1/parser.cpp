// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/parser.hpp"

#include "detail/error_format.hpp"
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

#include <filesystem>
#include <string_view>

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
    void CollectComponents(const YAML::Node& reaction, std::string_view key, std::vector<semantics::NamedRef>& out)
    {
      const std::string k(key);
      if (!reaction[k])
        return;
      for (const auto& item : AsSequence(reaction[k]))
        out.push_back({ GetComponentName(item), LocationOf(item) });
    }
  }  // namespace

  semantics::ReactionsInput BuildReactionsSemanticInput(const YAML::Node& object)
  {
    semantics::ReactionsInput input;

    if (object[std::string(keys::species)])
      for (const auto& s : object[std::string(keys::species)])
        input.species.push_back({ GetComponentName(s), LocationOf(s) });

    if (object[std::string(keys::phases)])
      for (const auto& phase : object[std::string(keys::phases)])
      {
        semantics::PhaseRef pr;
        pr.name = phase[std::string(keys::name)].as<std::string>();
        pr.location = LocationOf(phase);
        if (phase[std::string(keys::species)])
          for (const auto& ps : phase[std::string(keys::species)])
            pr.species.push_back({ GetComponentName(ps), LocationOf(ps) });
        input.phases.push_back(std::move(pr));
      }

    if (object[std::string(keys::reactions)])
      for (const auto& reaction : object[std::string(keys::reactions)])
      {
        semantics::ReactionRef rr;
        if (reaction[std::string(keys::type)])
          rr.type = reaction[std::string(keys::type)].as<std::string>();
        if (reaction[std::string(keys::gas_phase)])
        {
          rr.phase = reaction[std::string(keys::gas_phase)].as<std::string>();
          rr.location = LocationOf(reaction[std::string(keys::gas_phase)]);
        }
        // Reactant-like keys (must be in the reaction's phase).
        CollectComponents(reaction, keys::reactants, rr.reactants);
        CollectComponents(reaction, keys::gas_phase_species, rr.reactants);
        // Product-like keys (may reference any phase).
        CollectComponents(reaction, keys::products, rr.products);
        CollectComponents(reaction, keys::alkoxy_products, rr.products);
        CollectComponents(reaction, keys::nitrate_products, rr.products);
        CollectComponents(reaction, keys::gas_phase_products, rr.products);
        input.reactions.push_back(std::move(rr));
      }

    return input;
  }

  semantics::AerosolInput BuildAerosolSemanticInput(const YAML::Node& object)
  {
    semantics::AerosolInput input;

    if (object[std::string(keys::species)])
      for (const auto& s : object[std::string(keys::species)])
        input.species.push_back({ GetComponentName(s), static_cast<bool>(s[std::string(keys::molecular_weight)]) });

    if (object[std::string(keys::phases)])
      for (const auto& phase : object[std::string(keys::phases)])
      {
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
    if (!(object[std::string(keys::aerosol_representations)] && object[std::string(keys::aerosol_processes)]))
      return input;

    for (const auto& rep_node : object[std::string(keys::aerosol_representations)])
    {
      semantics::AerosolRepresentationRef ref;
      ref.name = rep_node[std::string(keys::name)].as<std::string>();
      ref.location = LocationOf(rep_node);
      if (rep_node[std::string(keys::phases)])
        for (const auto& phase_node : rep_node[std::string(keys::phases)])
          ref.phases.push_back({ phase_node.as<std::string>(), LocationOf(phase_node) });
      input.representations.push_back(std::move(ref));
    }

    for (const auto& entry : object[std::string(keys::aerosol_processes)])
    {
      const auto type = entry[std::string(keys::type)].as<std::string>();

      if (type == keys::HenrysLawPhaseTransfer_key)
      {
        semantics::HenrysLawPhaseTransferRef ref;
        ref.gas_phase = { entry[std::string(keys::gas_phase)].as<std::string>(),
                          LocationOf(entry[std::string(keys::gas_phase)]) };
        ref.gas_species = { entry[std::string(keys::gas_phase_species)].as<std::string>(),
                            LocationOf(entry[std::string(keys::gas_phase_species)]) };
        ref.condensed_phase = { entry[std::string(keys::condensed_phase)].as<std::string>(),
                                LocationOf(entry[std::string(keys::condensed_phase)]) };
        ref.condensed_species = { entry[std::string(keys::condensed_phase_species)].as<std::string>(),
                                  LocationOf(entry[std::string(keys::condensed_phase_species)]) };
        ref.solvent = { entry[std::string(keys::solvent)].as<std::string>(), LocationOf(entry[std::string(keys::solvent)]) };
        ref.location = LocationOf(entry);
        input.henrys_law_phase_transfers.push_back(std::move(ref));
      }
      else if (type == keys::DissolvedReaction_key)
      {
        semantics::DissolvedReactionRef ref;
        ref.phase = { entry[std::string(keys::condensed_phase)].as<std::string>(),
                      LocationOf(entry[std::string(keys::condensed_phase)]) };
        ref.solvent = { entry[std::string(keys::solvent)].as<std::string>(), LocationOf(entry[std::string(keys::solvent)]) };
        CollectComponents(entry, keys::reactants, ref.reactants);
        CollectComponents(entry, keys::products, ref.products);
        ref.location = LocationOf(entry);
        input.dissolved_reactions.push_back(std::move(ref));
      }
      else if (type == keys::DissolvedReversibleReaction_key)
      {
        semantics::DissolvedReversibleReactionRef ref;
        ref.phase = { entry[std::string(keys::condensed_phase)].as<std::string>(),
                      LocationOf(entry[std::string(keys::condensed_phase)]) };
        ref.solvent = { entry[std::string(keys::solvent)].as<std::string>(), LocationOf(entry[std::string(keys::solvent)]) };
        CollectComponents(entry, keys::reactants, ref.reactants);
        CollectComponents(entry, keys::products, ref.products);
        ref.location = LocationOf(entry);
        input.dissolved_reversible_reactions.push_back(std::move(ref));
      }
      else if (type == keys::HenrysLawEquilibrium_key)
      {
        semantics::HenrysLawEquilibriumRef ref;
        ref.gas_phase = { entry[std::string(keys::gas_phase)].as<std::string>(),
                          LocationOf(entry[std::string(keys::gas_phase)]) };
        ref.gas_species = { entry[std::string(keys::gas_phase_species)].as<std::string>(),
                            LocationOf(entry[std::string(keys::gas_phase_species)]) };
        ref.condensed_phase = { entry[std::string(keys::condensed_phase)].as<std::string>(),
                                LocationOf(entry[std::string(keys::condensed_phase)]) };
        ref.condensed_species = { entry[std::string(keys::condensed_phase_species)].as<std::string>(),
                                  LocationOf(entry[std::string(keys::condensed_phase_species)]) };
        ref.solvent = { entry[std::string(keys::solvent)].as<std::string>(), LocationOf(entry[std::string(keys::solvent)]) };
        ref.location = LocationOf(entry);
        input.henrys_law_equilibria.push_back(std::move(ref));
      }
      else if (type == keys::DissolvedEquilibrium_key)
      {
        semantics::DissolvedEquilibriumRef ref;
        ref.phase = { entry[std::string(keys::condensed_phase)].as<std::string>(),
                      LocationOf(entry[std::string(keys::condensed_phase)]) };
        ref.algebraic_species = { entry[std::string(keys::algebraic_species)].as<std::string>(),
                                  LocationOf(entry[std::string(keys::algebraic_species)]) };
        ref.solvent = { entry[std::string(keys::solvent)].as<std::string>(), LocationOf(entry[std::string(keys::solvent)]) };
        CollectComponents(entry, keys::reactants, ref.reactants);
        CollectComponents(entry, keys::products, ref.products);
        ref.location = LocationOf(entry);
        input.dissolved_equilibria.push_back(std::move(ref));
      }
      else if (type == keys::LinearConstraint_key)
      {
        semantics::LinearConstraintRef ref;
        ref.algebraic_phase = { entry[std::string(keys::algebraic_phase)].as<std::string>(),
                                LocationOf(entry[std::string(keys::algebraic_phase)]) };
        ref.algebraic_species = { entry[std::string(keys::algebraic_species)].as<std::string>(),
                                  LocationOf(entry[std::string(keys::algebraic_species)]) };
        if (entry[std::string(keys::terms)])
          for (const auto& term_node : entry[std::string(keys::terms)])
          {
            semantics::LinearConstraintTermRef term_ref;
            term_ref.phase = { term_node[std::string(keys::phase)].as<std::string>(),
                               LocationOf(term_node[std::string(keys::phase)]) };
            term_ref.species = { term_node[std::string(keys::name)].as<std::string>(),
                                 LocationOf(term_node[std::string(keys::name)]) };
            ref.terms.push_back(std::move(term_ref));
          }
        ref.location = LocationOf(entry);
        input.linear_constraints.push_back(std::move(ref));
      }
    }

    return input;
  }

  semantics::EmissionsInput BuildEmissionsSemanticInput(const YAML::Node& object)
  {
    semantics::EmissionsInput input;

    if (!object[std::string(keys::emissions)])
      return input;

    const YAML::Node& emissions_node = object[std::string(keys::emissions)];

    if (emissions_node[std::string(keys::inventories)])
      for (const auto& item : emissions_node[std::string(keys::inventories)])
        input.inventories.push_back({ item[std::string(keys::name)].as<std::string>(), LocationOf(item) });

    if (emissions_node[std::string(keys::species_maps)])
      for (const auto& item : emissions_node[std::string(keys::species_maps)])
      {
        semantics::SpeciesMapRef smap_ref;
        smap_ref.name = item[std::string(keys::name)].as<std::string>();
        smap_ref.location = LocationOf(item);
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

    if (emissions_node[std::string(keys::sources)])
      for (const auto& item : emissions_node[std::string(keys::sources)])
      {
        semantics::SourceRef source_ref;
        source_ref.name = item[std::string(keys::name)].as<std::string>();
        source_ref.location = LocationOf(item);
        source_ref.inventory = { item[std::string(keys::inventory)].as<std::string>(),
                                 LocationOf(item[std::string(keys::inventory)]) };
        source_ref.species_map = { item[std::string(keys::species_map)].as<std::string>(),
                                   LocationOf(item[std::string(keys::species_map)]) };
        if (item[std::string(keys::category)])
          source_ref.category = item[std::string(keys::category)].as<int>();
        if (item[std::string(keys::hierarchy)])
          source_ref.hierarchy = item[std::string(keys::hierarchy)].as<int>();
        input.sources.push_back(std::move(source_ref));
      }

    return input;
  }

  std::expected<YAML::Node, Errors> Parser::ResolveFileConfig(const std::filesystem::path& config_path)
  {
    if (!std::filesystem::exists(config_path) || !std::filesystem::is_regular_file(config_path))
    {
      return std::unexpected(Errors{
          { ErrorCode::FileNotFound,
            mc_fmt::format("Configuration file '{}' does not exist or is not a regular file.", config_path.string()) } });
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
          { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse '{}': {}", config_path.string(), e.what()) } });
    }

    Errors errors;
    const std::filesystem::path base_dir = config_path.parent_path();
    const Version version = object[keys::version] ? Version(object[keys::version].as<std::string>()) : Version();

    YAML::Node combined;
    if (object[keys::version])
      combined[std::string(keys::version)] = object[keys::version];
    if (object[keys::name])
      combined[std::string(keys::name)] = object[keys::name];

    // Loads and concatenates every file referenced under `<entity>.files`.
    auto load_files = [&](std::string_view entity) -> YAML::Node
    {
      YAML::Node merged(YAML::NodeType::Sequence);
      for (const auto& file_node : object[std::string(entity)]["files"])
      {
        const std::filesystem::path file_path = base_dir / file_node.as<std::string>();
        if (!std::filesystem::exists(file_path))
        {
          errors.push_back({ ErrorCode::FileNotFound, "File not found: " + file_path.string() });
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
          errors.push_back({ ErrorCode::UnexpectedError, "Failed to parse file: " + file_path.string() + ": " + e.what() });
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
            errors.push_back({ ErrorCode::RequiredKeyNotFound, "Missing 'files' key in '" + key + "' section." });
          else
            errors.push_back({ ErrorCode::InvalidType, "'" + key + "' must be a file-list object or an inline array." });
          break;
      }
    };
    // A file-list layout requires minor version >= 1; check before loading any files.
    const bool uses_filelist = (object[std::string(keys::species)] &&
                                GetEntityFormat(object[std::string(keys::species)]) == EntityFormat::FileList) ||
                               (object[std::string(keys::phases)] &&
                                GetEntityFormat(object[std::string(keys::phases)]) == EntityFormat::FileList) ||
                               (object[std::string(keys::reactions)] &&
                                GetEntityFormat(object[std::string(keys::reactions)]) == EntityFormat::FileList);
    if (uses_filelist && version.minor < 1)
    {
      errors.push_back({ ErrorCode::InvalidVersion,
                         "File-list format requires minor version >= 1, got " + std::to_string(version.minor) + "." });
      AppendFilePath(config_path_, errors);
      return std::unexpected(std::move(errors));
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
      AppendFilePath(config_path_, schema_errors);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      return errors;
    }

    // A config must define at least one mechanism mode: gas-phase reactions, or the aerosol
    // pair 'aerosol representations' + 'aerosol processes'. The two modes may also coexist.
    const bool has_reactions = static_cast<bool>(object[keys::reactions]);
    const bool has_aerosol_representations = static_cast<bool>(object[keys::aerosol_representations]);
    const bool has_aerosol_processes = static_cast<bool>(object[keys::aerosol_processes]);
    if (has_aerosol_representations != has_aerosol_processes)
    {
      ErrorLocation error_location{ object.Mark().line, object.Mark().column };
      errors.push_back({ ErrorCode::RequiredKeyNotFound,
                         mc_fmt::format(
                             "{} error: '{}' and '{}' must be provided together.",
                             error_location,
                             keys::aerosol_representations,
                             keys::aerosol_processes) });
    }
    if (!has_reactions && !(has_aerosol_representations && has_aerosol_processes))
    {
      ErrorLocation error_location{ object.Mark().line, object.Mark().column };
      errors.push_back({ ErrorCode::RequiredKeyNotFound,
                         mc_fmt::format(
                             "{} error: A configuration must contain either '{}' or both '{}' and '{}'.",
                             error_location,
                             keys::reactions,
                             keys::aerosol_representations,
                             keys::aerosol_processes) });
    }
    if (!errors.empty())
    {
      AppendFilePath(config_path_, errors);
      return errors;
    }

    constexpr unsigned int MAJOR_VERSION = 1;
    Version version = Version(object[keys::version].as<std::string>());
    if (version.major != MAJOR_VERSION)
    {
      ErrorLocation error_location{ object[keys::version].Mark().line, object[keys::version].Mark().column };

      std::string message = mc_fmt::format(
          "{} error: The version must be '{}' but the invalid version number '{}' found.",
          error_location,
          MAJOR_VERSION,
          version.major);
      errors.push_back({ ErrorCode::InvalidVersion, config_path_ + ":" + message });
    }

    // Species and phases are foundational. If either is invalid, fail fast since all downstream
    // validation depends on them.
    schema_errors = CheckSpeciesSchema(object[keys::species]);
    if (!schema_errors.empty())
    {
      AppendFilePath(config_path_, schema_errors);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      return errors;
    }

    auto parsed_species = ParseSpecies(object[keys::species]);

    schema_errors = CheckPhasesSchema(object[keys::phases], parsed_species);
    if (!schema_errors.empty())
    {
      AppendFilePath(config_path_, schema_errors);
      errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      return errors;
    }

    auto parsed_phases = ParsePhases(object[keys::phases]);

    // Gas-phase reactions are optional (an aerosol-only config may omit them).
    if (has_reactions)
    {
      schema_errors = CheckReactionsSchema(object[keys::reactions], parsed_species, parsed_phases);
      if (!schema_errors.empty())
      {
        AppendFilePath(config_path_, schema_errors);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }
    }

    // Aerosol sections are optional.
    if (has_aerosol_representations && has_aerosol_processes)
    {
      schema_errors = CheckAerosolRepresentationsSchema(object[keys::aerosol_representations]);
      if (!schema_errors.empty())
      {
        AppendFilePath(config_path_, schema_errors);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }

      schema_errors = CheckAerosolProcessesSchema(object[keys::aerosol_processes]);
      if (!schema_errors.empty())
      {
        AppendFilePath(config_path_, schema_errors);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }
    }

    if (object[std::string(keys::emissions)])
    {
      schema_errors = CheckEmissionsSchema(object[std::string(keys::emissions)]);
      if (!schema_errors.empty())
      {
        AppendFilePath(config_path_, schema_errors);
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
        return errors;
      }
    }

    return errors;
  }

  std::expected<Mechanism, Errors> Parser::Parse(const std::filesystem::path& config_path)
  {
    // ResolveFileConfig sets config_path_ so errors carry the file path.
    auto object = ResolveFileConfig(config_path);
    if (!object)
    {
      return std::unexpected(std::move(object.error()));
    }
    return ValidateAndBuild(*object);
  }

  std::expected<Mechanism, Errors> Parser::Parse(const std::string& content)
  {
    SetDefaultConfigPath();  // no file backing this document
    YAML::Node object;
    try
    {
      object = YAML::Load(content);
    }
    catch (const std::exception& e)
    {
      return std::unexpected(
          Errors{ { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse document: {}", e.what()) } });
    }
    return ValidateAndBuild(object);
  }

  std::expected<Mechanism, Errors> Parser::Parse(const YAML::Node& object)
  {
    SetDefaultConfigPath();  // no file backing this node
    return ValidateAndBuild(object);
  }

  std::expected<Mechanism, Errors> Parser::ValidateAndBuild(const YAML::Node& object)
  {
    try
    {
      // Structural (schema) validation.
      Errors errors = CheckSchema(object);

      // Semantic validation — needs a structurally-valid document, so only run it when
      // the structure is clean.
      if (errors.empty())
      {
        auto semantic_errors = ValidateReactionsSemantics(BuildReactionsSemanticInput(object));
        auto aerosol_errors = ValidateAerosolSemantics(BuildAerosolSemanticInput(object));
        auto emissions_errors = ValidateEmissionsSemantics(BuildEmissionsSemanticInput(object));
        semantic_errors.insert(semantic_errors.end(), aerosol_errors.begin(), aerosol_errors.end());
        semantic_errors.insert(semantic_errors.end(), emissions_errors.begin(), emissions_errors.end());
        AppendFilePath(config_path_, semantic_errors);
        errors.insert(errors.end(), semantic_errors.begin(), semantic_errors.end());
      }

      if (!errors.empty())
      {
        return std::unexpected(std::move(errors));
      }

      return Build(object);
    }
    catch (const std::exception& e)
    {
      const std::string where = config_path_.empty() ? "document" : "'" + config_path_ + "'";
      return std::unexpected(
          Errors{ { ErrorCode::UnexpectedError, mc_fmt::format("Failed to parse {}: {}", where, e.what()) } });
    }
  }

  Mechanism Parser::Build(const YAML::Node& object)
  {
    Mechanism mechanism;

    mechanism.version = Version(object[keys::version].as<std::string>());
    mechanism.species = ParseSpecies(object[keys::species]);
    mechanism.phases = ParsePhases(object[keys::phases]);

    if (object[keys::name])
    {
      mechanism.name = object[keys::name].as<std::string>();
    }
    if (object[keys::reactions])
    {
      mechanism.reactions = ParseReactions(object[keys::reactions]);
    }
    if (object[keys::aerosol_representations] && object[keys::aerosol_processes])
    {
      mechanism.aerosol = ParseAerosol(object, mechanism.species, mechanism.phases);
    }
    if (object[std::string(keys::emissions)])
    {
      mechanism.emissions = ParseEmissions(object[std::string(keys::emissions)]);
    }

    return mechanism;
  }

}  // namespace mechanism_configuration::v1

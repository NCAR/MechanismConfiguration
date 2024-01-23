// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/validation.hpp>
#include <open_atmos/mechanism_configuration/version.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    using nlohmann::json;

    std::string configParseStatusToString(const ConfigParseStatus& status)
    {
      switch (status)
      {
        case ConfigParseStatus::Success: return "Success";
        case ConfigParseStatus::None: return "None";
        case ConfigParseStatus::InvalidKey: return "InvalidKey";
        case ConfigParseStatus::UnknownKey: return "UnknownKey";
        case ConfigParseStatus::InvalidFilePath: return "InvalidFilePath";
        case ConfigParseStatus::ObjectTypeNotFound: return "ObjectTypeNotFound";
        case ConfigParseStatus::RequiredKeyNotFound: return "RequiredKeyNotFound";
        case ConfigParseStatus::MutuallyExclusiveOption: return "MutuallyExclusiveOption";
        case ConfigParseStatus::DuplicateSpeciesDetected: return "DuplicateSpeciesDetected";
        case ConfigParseStatus::DuplicatePhasesDetected: return "DuplicatePhasesDetected";
        case ConfigParseStatus::PhaseRequiresUnknownSpecies: return "PhaseRequiresUnknownSpecies";
        case ConfigParseStatus::ReactionRequiresUnknownSpecies: return "ReactionRequiresUnknownSpecies";
        case ConfigParseStatus::UnknownPhase: return "UnknownPhase";
        case ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase: return "RequestedAerosolSpeciesNotIncludedInAerosolPhase";
        case ConfigParseStatus::TooManyReactionComponents: return "TooManyReactionComponents";
        case ConfigParseStatus::InvalidIonPair: return "InvalidIonPair";
        default: return "Unknown";
      }
    }

    // Returns a vector for the allowed nonstandard keys, those that start with two underscores, like "__absolute tolerance"
    std::vector<std::string>
    GetComments(const json& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys)
    {
      // standard keys are:
      // those in required keys
      // those in optional keys
      // starting with __
      // anything else is reported as an error so that typos are caught, specifically for optional keys

      std::vector<std::string> sorted_object_keys;
      for (auto& [key, value] : object.items())
        sorted_object_keys.push_back(key);

      auto sorted_required_keys = required_keys;
      auto sorted_optional_keys = optional_keys;
      std::sort(sorted_object_keys.begin(), sorted_object_keys.end());
      std::sort(sorted_required_keys.begin(), sorted_required_keys.end());
      std::sort(sorted_optional_keys.begin(), sorted_optional_keys.end());

      // get the difference between the object keys and those required
      // what's left should be the optional keys and valid comments
      std::vector<std::string> difference;
      std::set_difference(
          sorted_object_keys.begin(),
          sorted_object_keys.end(),
          sorted_required_keys.begin(),
          sorted_required_keys.end(),
          std::back_inserter(difference));

      std::vector<std::string> remaining;
      std::set_difference(
          difference.begin(), difference.end(), sorted_optional_keys.begin(), sorted_optional_keys.end(), std::back_inserter(remaining));

      return remaining;
    }

    /// @brief Search for nonstandard keys. Only nonstandard keys starting with __ are allowed. Others are considered typos
    /// @param object the object whose keys need to be validated
    /// @param required_keys The required keys
    /// @param optional_keys The optional keys
    /// @return true if only standard keys are found
    ConfigParseStatus ValidateSchema(const json& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys)
    {
      // standard keys are:
      // those in required keys
      // those in optional keys
      // starting with __
      // anything else is reported as an error so that typos are caught, specifically for optional keys

      // debug statement
      // std::cout << "ValidateSchema object " << object.dump(4) << std::endl;

      if (!object.empty() && object.begin().value().is_null())
      {
        return ConfigParseStatus::Success;
      }

      std::vector<std::string> sorted_object_keys;
      for (auto& [key, value] : object.items())
        sorted_object_keys.push_back(key);

      auto sorted_required_keys = required_keys;
      auto sorted_optional_keys = optional_keys;
      std::sort(sorted_object_keys.begin(), sorted_object_keys.end());
      std::sort(sorted_required_keys.begin(), sorted_required_keys.end());
      std::sort(sorted_optional_keys.begin(), sorted_optional_keys.end());

      // get the difference between the object keys and those required
      // what's left should be the optional keys and valid comments
      std::vector<std::string> difference;
      std::set_difference(
          sorted_object_keys.begin(),
          sorted_object_keys.end(),
          sorted_required_keys.begin(),
          sorted_required_keys.end(),
          std::back_inserter(difference));

      // check that the number of keys remaining is exactly equal to the expected number of required keys
      if (difference.size() != (sorted_object_keys.size() - required_keys.size()))
      {
        std::vector<std::string> missing_keys;
        std::set_difference(
            sorted_required_keys.begin(),
            sorted_required_keys.end(),
            sorted_object_keys.begin(),
            sorted_object_keys.end(),
            std::back_inserter(missing_keys));
        for (auto& key : missing_keys)
          std::cerr << "Missing required key '" << key << "' in object: " << object << std::endl;

        return ConfigParseStatus::RequiredKeyNotFound;
      }

      std::vector<std::string> remaining;
      std::set_difference(
          difference.begin(), difference.end(), sorted_optional_keys.begin(), sorted_optional_keys.end(), std::back_inserter(remaining));

      // now, anything left must be standard comment starting with __
      for (auto& key : remaining)
      {
        if (key.find("__") == std::string::npos)
        {
          std::cerr << "Non-standard key '" << key << "' found in object" << object << std::endl;

          return ConfigParseStatus::InvalidKey;
        }
      }
      return ConfigParseStatus::Success;
    }

    template<typename T>
    bool ContainsUniqueObjectsByName(const std::vector<T>& collection)
    {
      for (size_t i = 0; i < collection.size(); ++i)
      {
        for (size_t j = i + 1; j < collection.size(); ++j)
        {
          if (collection[i].name == collection[j].name)
          {
            return false;
          }
        }
      }
      return true;
    }

    /// @brief Check if any species in required by a reaction is not in the species configured for this mechanism
    /// @param requested_species a list of species needed by a reaction
    /// @param existing_species all configured species in the mechanism
    /// @return a boolean indicating if this reaction needs species that are not configured in the mechanism
    bool RequiresUnknownSpecies(const std::vector<std::string> requested_species, const std::vector<types::Species>& existing_species)
    {
      for (const auto& spec : requested_species)
      {
        auto it =
            std::find_if(existing_species.begin(), existing_species.end(), [&spec](const types::Species& existing) { return existing.name == spec; });

        if (it == existing_species.end())
        {
          return true;
        }
      }
      return false;
    }

    /// @brief Check if any species in required by a reaction is not in the species configured for this mechanism
    /// @param requested_species a list of species needed by a reaction
    /// @param existing_species all configured species in the mechanism
    /// @return a boolean indicating if this reaction needs species that are not configured in the mechanism
    bool RequiresUnknownSpecies(const std::vector<std::string> requested_species, const std::vector<std::string>& existing_species)
    {
      for (const auto& spec : requested_species)
      {
        auto it = std::find_if(existing_species.begin(), existing_species.end(), [&spec](const std::string& existing) { return existing == spec; });

        if (it == existing_species.end())
        {
          return true;
        }
      }
      return false;
    }

    /// @brief Parses species in a mechanism
    /// @param objects json object of species
    /// @return A pair indicating parsing success and a list of species
    std::pair<ConfigParseStatus, std::vector<types::Species>> ParseSpecies(const json& objects)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      std::vector<types::Species> all_species;

      for (const auto& object : objects)
      {
        types::Species species;
        status = ValidateSchema(object, validation::species.required_keys, validation::species.optional_keys);
        if (status != ConfigParseStatus::Success)
        {
          break;
        }

        std::string name = object[validation::keys.name].get<std::string>();

        std::map<std::string, double> numerical_properties{};
        for (const auto& key : validation::species.optional_keys)
        {
          if (object.contains(key))
          {
            double val = object[key].get<double>();
            numerical_properties[key] = val;
          }
        }

        auto comments = GetComments(object, validation::species.required_keys, validation::species.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        species.name = name;
        species.optional_numerical_properties = numerical_properties;
        species.unknown_properties = unknown_properties;

        all_species.push_back(species);
      }

      if (!ContainsUniqueObjectsByName<types::Species>(all_species))
        status = ConfigParseStatus::DuplicateSpeciesDetected;

      return { status, all_species };
    }

    /// @brief Parses phases in a mechanism
    /// @param objects jsoon object of phases
    /// @param existing_species a list of species configured in the mechanism
    /// @return A pair indicating parsing success and a list of phases
    std::pair<ConfigParseStatus, std::vector<types::Phase>> ParsePhases(const json& objects, const std::vector<types::Species> existing_species)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      std::vector<types::Phase> all_phases;

      for (const auto& object : objects)
      {
        types::Phase phase;
        status = ValidateSchema(object, validation::phase.required_keys, validation::phase.optional_keys);
        if (status != ConfigParseStatus::Success)
        {
          break;
        }

        std::string name = object[validation::keys.name].get<std::string>();

        std::vector<std::string> species{};
        for (const auto& spec : object[validation::keys.species])
        {
          species.push_back(spec);
        }

        auto comments = GetComments(object, validation::phase.required_keys, validation::phase.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        phase.name = name;
        phase.species = species;
        phase.unknown_properties = unknown_properties;

        if (RequiresUnknownSpecies(species, existing_species))
        {
          status = ConfigParseStatus::PhaseRequiresUnknownSpecies;
          break;
        }

        all_phases.push_back(phase);
      }

      if (status == ConfigParseStatus::Success && !ContainsUniqueObjectsByName<types::Phase>(all_phases))
        status = ConfigParseStatus::DuplicatePhasesDetected;

      return { status, all_phases };
    }

    /// @brief Parses all reactions
    /// @param object
    /// @return A pair indicating parsing success and a struct of all reactions found in the mechanism
    std::pair<ConfigParseStatus, types::ReactionComponent> ParseReactionComponent(const json& object)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::ReactionComponent component;

      status = ValidateSchema(object, validation::reaction_component.required_keys, validation::reaction_component.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::string species_name = object[validation::keys.species_name].get<std::string>();
        double coefficient = 1;
        if (object.contains(validation::keys.coefficient))
        {
          coefficient = object[validation::keys.coefficient].get<double>();
        }

        auto comments = GetComments(object, validation::reaction_component.required_keys, validation::reaction_component.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        component.species_name = species_name;
        component.coefficient = coefficient;
        component.unknown_properties = unknown_properties;
      }

      return { status, component };
    }

    /// @brief Parses an arrhenius reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of Arrhenius parameters
    std::pair<ConfigParseStatus, types::Arrhenius>
    ParseArrhenius(const json& object, const std::vector<types::Species>& existing_species, const std::vector<types::Phase> existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Arrhenius arrhenius;

      status = ValidateSchema(object, validation::arrhenius.required_keys, validation::arrhenius.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::vector<types::ReactionComponent> products{};
        for (const auto& product : object[validation::keys.products])
        {
          auto product_parse = ParseReactionComponent(product);
          status = product_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          products.push_back(product_parse.second);
        }

        std::vector<types::ReactionComponent> reactants{};
        for (const auto& reactant : object[validation::keys.reactants])
        {
          auto reactant_parse = ParseReactionComponent(reactant);
          status = reactant_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactants.push_back(reactant_parse.second);
        }

        if (object.contains(validation::keys.A))
        {
          arrhenius.A = object[validation::keys.A].get<double>();
        }
        if (object.contains(validation::keys.B))
        {
          arrhenius.B = object[validation::keys.B].get<double>();
        }
        if (object.contains(validation::keys.C))
        {
          arrhenius.C = object[validation::keys.C].get<double>();
        }
        if (object.contains(validation::keys.D))
        {
          arrhenius.D = object[validation::keys.D].get<double>();
        }
        if (object.contains(validation::keys.E))
        {
          arrhenius.E = object[validation::keys.E].get<double>();
        }
        if (object.contains(validation::keys.Ea))
        {
          if (arrhenius.C != 0)
          {
            std::cerr << "Ea is specified when C is also specified for an Arrhenius reaction. Pick one." << std::endl;
            status = ConfigParseStatus::MutuallyExclusiveOption;
          }
          // Calculate 'C' using 'Ea'
          arrhenius.C = -1 * object[validation::keys.Ea].get<double>() / constants::boltzmann;
        }

        if (object.contains(validation::keys.name))
        {
          arrhenius.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::arrhenius.required_keys, validation::arrhenius.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::vector<std::string> requested_species;
        for (const auto& spec : products)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants)
        {
          requested_species.push_back(spec.species_name);
        }

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        std::string gas_phase = object[validation::keys.gas_phase].get<std::string>();
        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (status == ConfigParseStatus::Success && it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        arrhenius.gas_phase = gas_phase;
        arrhenius.products = products;
        arrhenius.reactants = reactants;
        arrhenius.unknown_properties = unknown_properties;
      }

      return { status, arrhenius };
    }

    /// @brief Parses a condensed phase arrhenius reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of Condensed Phase Arrhenius parameters
    std::pair<ConfigParseStatus, types::CondensedPhaseArrhenius> ParseCondensedPhaseArrhenius(
        const json& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::CondensedPhaseArrhenius condensed_phase_arrhenius;

      status = ValidateSchema(object, validation::condensed_phase_arrhenius.required_keys, validation::condensed_phase_arrhenius.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::vector<types::ReactionComponent> products{};
        for (const auto& product : object[validation::keys.products])
        {
          auto product_parse = ParseReactionComponent(product);
          status = product_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          products.push_back(product_parse.second);
        }

        std::vector<types::ReactionComponent> reactants{};
        for (const auto& reactant : object[validation::keys.reactants])
        {
          auto reactant_parse = ParseReactionComponent(reactant);
          status = reactant_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactants.push_back(reactant_parse.second);
        }

        if (object.contains(validation::keys.A))
        {
          condensed_phase_arrhenius.A = object[validation::keys.A].get<double>();
        }
        if (object.contains(validation::keys.B))
        {
          condensed_phase_arrhenius.B = object[validation::keys.B].get<double>();
        }
        if (object.contains(validation::keys.C))
        {
          condensed_phase_arrhenius.C = object[validation::keys.C].get<double>();
        }
        if (object.contains(validation::keys.D))
        {
          condensed_phase_arrhenius.D = object[validation::keys.D].get<double>();
        }
        if (object.contains(validation::keys.E))
        {
          condensed_phase_arrhenius.E = object[validation::keys.E].get<double>();
        }
        if (object.contains(validation::keys.Ea))
        {
          if (condensed_phase_arrhenius.C != 0)
          {
            std::cerr << "Ea is specified when C is also specified for an CondensedPhasecondensed_phase_arrhenius reaction. Pick one." << std::endl;
            status = ConfigParseStatus::MutuallyExclusiveOption;
          }
          // Calculate 'C' using 'Ea'
          condensed_phase_arrhenius.C = -1 * object[validation::keys.Ea].get<double>() / constants::boltzmann;
        }

        if (object.contains(validation::keys.name))
        {
          condensed_phase_arrhenius.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::condensed_phase_arrhenius.required_keys, validation::condensed_phase_arrhenius.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::string aerosol_phase = object[validation::keys.aerosol_phase].get<std::string>();
        std::string aerosol_phase_water = object[validation::keys.aerosol_phase_water].get<std::string>();

        std::vector<std::string> requested_species;
        for (const auto& spec : products)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants)
        {
          requested_species.push_back(spec.species_name);
        }
        requested_species.push_back(aerosol_phase_water);

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        auto phase_it = std::find_if(
            existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const types::Phase& phase) { return phase.name == aerosol_phase; });

        if (phase_it != existing_phases.end())
        {
          // check if all of the species for this reaction are actually in the aerosol phase
          std::vector<std::string> aerosol_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, aerosol_phase_species))
          {
            status = ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase;
          }
        }
        else
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        condensed_phase_arrhenius.aerosol_phase = aerosol_phase;
        condensed_phase_arrhenius.aerosol_phase_water = aerosol_phase_water;
        condensed_phase_arrhenius.products = products;
        condensed_phase_arrhenius.reactants = reactants;
        condensed_phase_arrhenius.unknown_properties = unknown_properties;
      }

      return { status, condensed_phase_arrhenius };
    }

    /// @brief Parses a troe reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of Troe parameters
    std::pair<ConfigParseStatus, types::Troe>
    ParseTroe(const json& object, const std::vector<types::Species>& existing_species, const std::vector<types::Phase> existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Troe troe;

      status = ValidateSchema(object, validation::troe.required_keys, validation::troe.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::vector<types::ReactionComponent> products{};
        for (const auto& product : object[validation::keys.products])
        {
          auto product_parse = ParseReactionComponent(product);
          status = product_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          products.push_back(product_parse.second);
        }

        std::vector<types::ReactionComponent> reactants{};
        for (const auto& reactant : object[validation::keys.reactants])
        {
          auto reactant_parse = ParseReactionComponent(reactant);
          status = reactant_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactants.push_back(reactant_parse.second);
        }

        if (object.contains(validation::keys.k0_A))
        {
          troe.k0_A = object[validation::keys.k0_A].get<double>();
        }
        if (object.contains(validation::keys.k0_B))
        {
          troe.k0_B = object[validation::keys.k0_B].get<double>();
        }
        if (object.contains(validation::keys.k0_C))
        {
          troe.k0_C = object[validation::keys.k0_C].get<double>();
        }
        if (object.contains(validation::keys.kinf_A))
        {
          troe.kinf_A = object[validation::keys.kinf_A].get<double>();
        }
        if (object.contains(validation::keys.kinf_B))
        {
          troe.kinf_B = object[validation::keys.kinf_B].get<double>();
        }
        if (object.contains(validation::keys.kinf_C))
        {
          troe.kinf_C = object[validation::keys.kinf_C].get<double>();
        }
        if (object.contains(validation::keys.Fc))
        {
          troe.Fc = object[validation::keys.Fc].get<double>();
        }
        if (object.contains(validation::keys.N))
        {
          troe.N = object[validation::keys.N].get<double>();
        }

        if (object.contains(validation::keys.name))
        {
          troe.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::troe.required_keys, validation::troe.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::vector<std::string> requested_species;
        for (const auto& spec : products)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants)
        {
          requested_species.push_back(spec.species_name);
        }

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        std::string gas_phase = object[validation::keys.gas_phase].get<std::string>();
        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (status == ConfigParseStatus::Success && it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        troe.gas_phase = gas_phase;
        troe.products = products;
        troe.reactants = reactants;
        troe.unknown_properties = unknown_properties;
      }

      return { status, troe };
    }

    /// @brief Parses a branched reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of Branched parameters
    std::pair<ConfigParseStatus, types::Branched>
    ParseBranched(const json& object, const std::vector<types::Species>& existing_species, const std::vector<types::Phase> existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Branched branched;

      status = ValidateSchema(object, validation::branched.required_keys, validation::branched.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::vector<types::ReactionComponent> alkoxy_products{};
        for (const auto& product : object[validation::keys.alkoxy_products])
        {
          auto product_parse = ParseReactionComponent(product);
          status = product_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          alkoxy_products.push_back(product_parse.second);
        }

        std::vector<types::ReactionComponent> nitrate_products{};
        for (const auto& product : object[validation::keys.nitrate_products])
        {
          auto product_parse = ParseReactionComponent(product);
          status = product_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          nitrate_products.push_back(product_parse.second);
        }

        std::vector<types::ReactionComponent> reactants{};
        for (const auto& reactant : object[validation::keys.reactants])
        {
          auto reactant_parse = ParseReactionComponent(reactant);
          status = reactant_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactants.push_back(reactant_parse.second);
        }

        branched.X = object[validation::keys.X].get<double>();
        branched.Y = object[validation::keys.Y].get<double>();
        branched.a0 = object[validation::keys.a0].get<double>();
        branched.n = object[validation::keys.n].get<double>();

        if (object.contains(validation::keys.name))
        {
          branched.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::branched.required_keys, validation::branched.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::vector<std::string> requested_species;
        for (const auto& spec : nitrate_products)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : alkoxy_products)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants)
        {
          requested_species.push_back(spec.species_name);
        }

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        std::string gas_phase = object[validation::keys.gas_phase].get<std::string>();
        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (status == ConfigParseStatus::Success && it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        branched.gas_phase = gas_phase;
        branched.nitrate_products = nitrate_products;
        branched.alkoxy_products = alkoxy_products;
        branched.reactants = reactants;
        branched.unknown_properties = unknown_properties;
      }

      return { status, branched };
    }

    /// @brief Parses a tunneling reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of Tunneling parameters
    std::pair<ConfigParseStatus, types::Tunneling>
    ParseTunneling(const json& object, const std::vector<types::Species>& existing_species, const std::vector<types::Phase> existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Tunneling tunneling;

      status = ValidateSchema(object, validation::tunneling.required_keys, validation::tunneling.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::vector<types::ReactionComponent> products{};
        for (const auto& product : object[validation::keys.products])
        {
          auto product_parse = ParseReactionComponent(product);
          status = product_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          products.push_back(product_parse.second);
        }

        std::vector<types::ReactionComponent> reactants{};
        for (const auto& reactant : object[validation::keys.reactants])
        {
          auto reactant_parse = ParseReactionComponent(reactant);
          status = reactant_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactants.push_back(reactant_parse.second);
        }

        if (object.contains(validation::keys.A))
        {
          tunneling.A = object[validation::keys.A].get<double>();
        }
        if (object.contains(validation::keys.B))
        {
          tunneling.B = object[validation::keys.B].get<double>();
        }
        if (object.contains(validation::keys.C))
        {
          tunneling.C = object[validation::keys.C].get<double>();
        }

        if (object.contains(validation::keys.name))
        {
          tunneling.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::tunneling.required_keys, validation::tunneling.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::vector<std::string> requested_species;
        for (const auto& spec : products)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants)
        {
          requested_species.push_back(spec.species_name);
        }

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        std::string gas_phase = object[validation::keys.gas_phase].get<std::string>();
        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (status == ConfigParseStatus::Success && it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        tunneling.gas_phase = gas_phase;
        tunneling.products = products;
        tunneling.reactants = reactants;
        tunneling.unknown_properties = unknown_properties;
      }

      return { status, tunneling };
    }

    /// @brief Parses a surface reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of Surface parameters
    std::pair<ConfigParseStatus, types::Surface>
    ParseSurface(const json& object, const std::vector<types::Species>& existing_species, const std::vector<types::Phase> existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Surface surface;

      status = ValidateSchema(object, validation::surface.required_keys, validation::surface.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::string gas_phase_species = object[validation::keys.gas_phase_species].get<std::string>();

        std::vector<types::ReactionComponent> products{};
        for (const auto& reactant : object[validation::keys.gas_phase_products])
        {
          auto product_parse = ParseReactionComponent(reactant);
          status = product_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          products.push_back(product_parse.second);
        }

        if (object.contains(validation::keys.reaction_probability))
        {
          surface.reaction_probability = object[validation::keys.reaction_probability].get<double>();
        }

        if (object.contains(validation::keys.name))
        {
          surface.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::surface.required_keys, validation::surface.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::vector<std::string> requested_species;
        for (const auto& spec : products)
        {
          requested_species.push_back(spec.species_name);
        }
        requested_species.push_back(gas_phase_species);

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }


        std::string aerosol_phase = object[validation::keys.aerosol_phase].get<std::string>();
        auto it =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const auto& phase) { return phase.name == aerosol_phase; });
        if (status == ConfigParseStatus::Success && it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        surface.gas_phase = object[validation::keys.gas_phase].get<std::string>();
        surface.aerosol_phase = aerosol_phase;
        surface.gas_phase_products = products;
        surface.gas_phase_species = { .species_name = gas_phase_species, .coefficient = 1 };
        surface.unknown_properties = unknown_properties;
      }

      return { status, surface };
    }

    /// @brief Parses a photolysis reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of Photolysis parameters
    std::pair<ConfigParseStatus, types::Photolysis>
    ParsePhotolysis(const json& object, const std::vector<types::Species> existing_species, const std::vector<types::Phase> existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Photolysis photolysis;

      status = ValidateSchema(object, validation::photolysis.required_keys, validation::photolysis.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::vector<types::ReactionComponent> products{};
        for (const auto& reactant : object[validation::keys.products])
        {
          auto product_parse = ParseReactionComponent(reactant);
          status = product_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          products.push_back(product_parse.second);
        }

        std::vector<types::ReactionComponent> reactants{};
        for (const auto& reactant : object[validation::keys.reactants])
        {
          auto reactant_parse = ParseReactionComponent(reactant);
          status = reactant_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactants.push_back(reactant_parse.second);
        }

        if (object.contains(validation::keys.scaling_factor))
        {
          photolysis.scaling_factor = object[validation::keys.scaling_factor].get<double>();
        }

        if (object.contains(validation::keys.name))
        {
          photolysis.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::photolysis.required_keys, validation::photolysis.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::vector<std::string> requested_species;
        for (const auto& spec : products)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants)
        {
          requested_species.push_back(spec.species_name);
        }

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        std::string gas_phase = object[validation::keys.gas_phase].get<std::string>();
        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (status == ConfigParseStatus::Success && it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        if (status == ConfigParseStatus::Success && reactants.size() > 1)
        {
          status = ConfigParseStatus::TooManyReactionComponents;
        }

        photolysis.gas_phase = gas_phase;
        photolysis.products = products;
        photolysis.reactants = reactants;
        photolysis.unknown_properties = unknown_properties;
      }

      return { status, photolysis };
    }

    /// @brief Parses a photolysis reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of Photolysis parameters
    std::pair<ConfigParseStatus, types::CondensedPhasePhotolysis> ParseCondensedPhasePhotolysis(
        const json& object,
        const std::vector<types::Species> existing_species,
        const std::vector<types::Phase> existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::CondensedPhasePhotolysis condensed_phase_photolysis;

      status = ValidateSchema(object, validation::condensed_phase_photolysis.required_keys, validation::photolysis.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::vector<types::ReactionComponent> products{};
        for (const auto& reactant : object[validation::keys.products])
        {
          auto product_parse = ParseReactionComponent(reactant);
          status = product_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          products.push_back(product_parse.second);
        }

        std::vector<types::ReactionComponent> reactants{};
        for (const auto& reactant : object[validation::keys.reactants])
        {
          auto reactant_parse = ParseReactionComponent(reactant);
          status = reactant_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactants.push_back(reactant_parse.second);
        }

        if (object.contains(validation::keys.scaling_factor))
        {
          condensed_phase_photolysis.scaling_factor_ = object[validation::keys.scaling_factor].get<double>();
        }

        if (object.contains(validation::keys.name))
        {
          condensed_phase_photolysis.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::condensed_phase_photolysis.required_keys, validation::photolysis.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::string aerosol_phase = object[validation::keys.aerosol_phase].get<std::string>();
        std::string aerosol_phase_water = object[validation::keys.aerosol_phase_water].get<std::string>();

        std::vector<std::string> requested_species;
        for (const auto& spec : products)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants)
        {
          requested_species.push_back(spec.species_name);
        }
        requested_species.push_back(aerosol_phase_water);

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        if (status == ConfigParseStatus::Success && reactants.size() > 1)
        {
          status = ConfigParseStatus::TooManyReactionComponents;
        }

        auto phase_it = std::find_if(
            existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const types::Phase& phase) { return phase.name == aerosol_phase; });

        if (phase_it != existing_phases.end())
        {
          // check if all of the species for this reaction are actually in the aerosol phase
          std::vector<std::string> aerosol_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, aerosol_phase_species))
          {
            status = ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase;
          }
        }
        else
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        condensed_phase_photolysis.aerosol_phase = aerosol_phase;
        condensed_phase_photolysis.aerosol_phase_water = aerosol_phase_water;
        condensed_phase_photolysis.products = products;
        condensed_phase_photolysis.reactants = reactants;
        condensed_phase_photolysis.unknown_properties = unknown_properties;
      }

      return { status, condensed_phase_photolysis };
    }

    /// @brief Parses a emission reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of Emission parameters
    std::pair<ConfigParseStatus, types::Emission>
    ParseEmission(const json& object, const std::vector<types::Species> existing_species, const std::vector<types::Phase> existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Emission emission;

      status = ValidateSchema(object, validation::emission.required_keys, validation::emission.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::vector<types::ReactionComponent> products{};
        for (const auto& product : object[validation::keys.products])
        {
          auto product_parse = ParseReactionComponent(product);
          status = product_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          products.push_back(product_parse.second);
        }

        if (object.contains(validation::keys.scaling_factor))
        {
          emission.scaling_factor = object[validation::keys.scaling_factor].get<double>();
        }

        if (object.contains(validation::keys.name))
        {
          emission.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::emission.required_keys, validation::emission.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::vector<std::string> requested_species;
        for (const auto& spec : products)
        {
          requested_species.push_back(spec.species_name);
        }

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        std::string gas_phase = object[validation::keys.gas_phase].get<std::string>();
        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (status == ConfigParseStatus::Success && it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        emission.gas_phase = gas_phase;
        emission.products = products;
        emission.unknown_properties = unknown_properties;
      }

      return { status, emission };
    }

    /// @brief Parses a first order loss reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of First Order Loss parameters
    std::pair<ConfigParseStatus, types::FirstOrderLoss>
    ParseFirstOrderLoss(const json& object, const std::vector<types::Species> existing_species, const std::vector<types::Phase> existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::FirstOrderLoss first_order_loss;

      status = ValidateSchema(object, validation::first_order_loss.required_keys, validation::first_order_loss.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::vector<types::ReactionComponent> reactants{};
        for (const auto& reactant : object[validation::keys.reactants])
        {
          auto reactant_parse = ParseReactionComponent(reactant);
          status = reactant_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactants.push_back(reactant_parse.second);
        }

        if (object.contains(validation::keys.scaling_factor))
        {
          first_order_loss.scaling_factor = object[validation::keys.scaling_factor].get<double>();
        }

        if (object.contains(validation::keys.name))
        {
          first_order_loss.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::first_order_loss.required_keys, validation::first_order_loss.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::vector<std::string> requested_species;
        for (const auto& spec : reactants)
        {
          requested_species.push_back(spec.species_name);
        }

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        std::string gas_phase = object[validation::keys.gas_phase].get<std::string>();
        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (status == ConfigParseStatus::Success && it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        if (status == ConfigParseStatus::Success && reactants.size() > 1)
        {
          status = ConfigParseStatus::TooManyReactionComponents;
        }

        first_order_loss.gas_phase = gas_phase;
        first_order_loss.reactants = reactants;
        first_order_loss.unknown_properties = unknown_properties;
      }

      return { status, first_order_loss };
    }

    /// @brief Parses a SIMPOL phase transfer reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of Surface parameters
    std::pair<ConfigParseStatus, types::SimpolPhaseTransfer>
    ParseSimpolPhaseTransfer(const json& object, const std::vector<types::Species>& existing_species, const std::vector<types::Phase> existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::SimpolPhaseTransfer simpol_phase_transfer;

      status = ValidateSchema(object, validation::simpol_phase_transfer.required_keys, validation::simpol_phase_transfer.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::string gas_phase_species = object[validation::keys.gas_phase_species].get<std::string>();
        std::string aerosol_phase_species = object[validation::keys.aerosol_phase_species].get<std::string>();

        if (object.contains(validation::keys.name))
        {
          simpol_phase_transfer.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::simpol_phase_transfer.required_keys, validation::simpol_phase_transfer.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::vector<std::string> requested_species {gas_phase_species, aerosol_phase_species};
        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        std::string aerosol_phase = object[validation::keys.aerosol_phase].get<std::string>();
        auto aerosol_it =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const auto& phase) { return phase.name == aerosol_phase; });
        if (status == ConfigParseStatus::Success && aerosol_it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }
        else {
          auto phase = *aerosol_it;
          auto spec_it = std::find_if(phase.species.begin(), phase.species.end(), [&aerosol_phase_species](const std::string& species){
            return species == aerosol_phase_species;
          });
          if (spec_it == phase.species.end()) {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }
        }

        std::string gas_phase = object[validation::keys.gas_phase].get<std::string>();
        auto gas_it =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (status == ConfigParseStatus::Success && gas_it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }
        else {
          auto phase = *gas_it;
          auto spec_it = std::find_if(phase.species.begin(), phase.species.end(), [&gas_phase_species](const std::string& species){
            return species == gas_phase_species;
          });
          if (spec_it == phase.species.end()) {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }
        }

        if (object.contains(validation::keys.B) && object[validation::keys.B].is_array() && object[validation::keys.B].size() == 4) {
          for(size_t i = 0; i < 4; ++i) {
            simpol_phase_transfer.B[i] = object[validation::keys.B][i];
          }
        }

        simpol_phase_transfer.gas_phase = gas_phase;
        simpol_phase_transfer.gas_phase_species = { .species_name = gas_phase_species, .coefficient = 1 };
        simpol_phase_transfer.aerosol_phase = aerosol_phase;
        simpol_phase_transfer.aerosol_phase_species = { .species_name = aerosol_phase_species, .coefficient = 1 };
        simpol_phase_transfer.unknown_properties = unknown_properties;
      }

      return { status, simpol_phase_transfer };
    }

    /// @brief Parses an aqueous equilibrium reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of Condensed Phase Arrhenius parameters
    std::pair<ConfigParseStatus, types::AqueousEquilibrium> ParseAqueousEquilibrium(
        const json& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::AqueousEquilibrium aqueous_equilibrium;

      status = ValidateSchema(object, validation::aqueous_equilibrium.required_keys, validation::aqueous_equilibrium.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::vector<types::ReactionComponent> products{};
        for (const auto& product : object[validation::keys.products])
        {
          auto product_parse = ParseReactionComponent(product);
          status = product_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          products.push_back(product_parse.second);
        }

        std::vector<types::ReactionComponent> reactants{};
        for (const auto& reactant : object[validation::keys.reactants])
        {
          auto reactant_parse = ParseReactionComponent(reactant);
          status = reactant_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactants.push_back(reactant_parse.second);
        }

        if (object.contains(validation::keys.A))
        {
          aqueous_equilibrium.A = object[validation::keys.A].get<double>();
        }
        if (object.contains(validation::keys.C))
        {
          aqueous_equilibrium.C = object[validation::keys.C].get<double>();
        }

        aqueous_equilibrium.k_reverse = object[validation::keys.k_reverse].get<double>();

        if (object.contains(validation::keys.name))
        {
          aqueous_equilibrium.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::aqueous_equilibrium.required_keys, validation::aqueous_equilibrium.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::string aerosol_phase = object[validation::keys.aerosol_phase].get<std::string>();
        std::string aerosol_phase_water = object[validation::keys.aerosol_phase_water].get<std::string>();

        std::vector<std::string> requested_species;
        for (const auto& spec : products)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants)
        {
          requested_species.push_back(spec.species_name);
        }
        requested_species.push_back(aerosol_phase_water);

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        auto phase_it = std::find_if(
            existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const types::Phase& phase) { return phase.name == aerosol_phase; });

        if (phase_it != existing_phases.end())
        {
          // check if all of the species for this reaction are actually in the aerosol phase
          std::vector<std::string> aerosol_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, aerosol_phase_species))
          {
            status = ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase;
          }
        }
        else
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        aqueous_equilibrium.aerosol_phase = aerosol_phase;
        aqueous_equilibrium.aerosol_phase_water = aerosol_phase_water;
        aqueous_equilibrium.products = products;
        aqueous_equilibrium.reactants = reactants;
        aqueous_equilibrium.unknown_properties = unknown_properties;
      }

      return { status, aqueous_equilibrium };
    }

    /// @brief Parses a wet deposition reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of First Order Loss parameters
    std::pair<ConfigParseStatus, types::WetDeposition>
    ParseWetDeposition(const json& object, const std::vector<types::Species> existing_species, const std::vector<types::Phase> existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::WetDeposition wet_deposition;

      status = ValidateSchema(object, validation::wet_deposition.required_keys, validation::wet_deposition.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        if (object.contains(validation::keys.scaling_factor))
        {
          wet_deposition.scaling_factor = object[validation::keys.scaling_factor].get<double>();
        }

        if (object.contains(validation::keys.name))
        {
          wet_deposition.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::wet_deposition.required_keys, validation::wet_deposition.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::string aerosol_phase = object[validation::keys.aerosol_phase].get<std::string>();

        // check if aerosol phase exists
        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const auto& phase) { return phase.name == aerosol_phase; });
        if (status == ConfigParseStatus::Success && it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        wet_deposition.aerosol_phase = aerosol_phase;
        wet_deposition.unknown_properties = unknown_properties;
      }

      return { status, wet_deposition };
    }

    /// @brief Parses a first order loss reaction
    /// @param object A json object that should have information containing arrhenius parameters
    /// @param existing_species A list of species configured in a mechanism
    /// @param existing_phases A list of phases configured in a mechanism
    /// @return A pair indicating parsing success and a struct of First Order Loss parameters
    std::pair<ConfigParseStatus, types::HenrysLaw>
    ParseHenrysLaw(const json& object, const std::vector<types::Species> existing_species, const std::vector<types::Phase> existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::HenrysLaw henrys_law;

      status = ValidateSchema(object, validation::henrys_law.required_keys, validation::henrys_law.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::string gas_phase = object[validation::keys.gas_phase].get<std::string>();
        std::string gas_phase_species = object[validation::keys.gas_phase_species].get<std::string>();
        std::string aerosol_phase = object[validation::keys.aerosol_phase].get<std::string>();
        std::string aerosol_phase_species = object[validation::keys.aerosol_phase_species].get<std::string>();
        std::string aerosol_phase_water = object[validation::keys.aerosol_phase_water].get<std::string>();

        if (object.contains(validation::keys.name))
        {
          henrys_law.name = object[validation::keys.name].get<std::string>();
        }

        auto comments = GetComments(object, validation::henrys_law.required_keys, validation::henrys_law.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        std::vector<std::string> requested_species;
        requested_species.push_back(gas_phase_species);
        requested_species.push_back(aerosol_phase_species);
        requested_species.push_back(aerosol_phase_water);

        std::vector<std::string> requested_aerosol_species;
        requested_aerosol_species.push_back(aerosol_phase_species);
        requested_aerosol_species.push_back(aerosol_phase_water);

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (status == ConfigParseStatus::Success && it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        auto phase_it = std::find_if(
            existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const types::Phase& phase) { return phase.name == aerosol_phase; });

        if (phase_it != existing_phases.end())
        {
          // check if all of the species for this reaction are actually in the aerosol phase
          std::vector<std::string> aerosol_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_aerosol_species, aerosol_phase_species))
          {
            status = ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase;
          }
        }
        else
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        henrys_law.gas_phase = gas_phase;
        henrys_law.gas_phase_species = gas_phase_species;
        henrys_law.aerosol_phase = aerosol_phase;
        henrys_law.aerosol_phase_species = aerosol_phase_species;
        henrys_law.aerosol_phase_water = aerosol_phase_water;
        henrys_law.unknown_properties = unknown_properties;
      }

      return { status, henrys_law };
    }

    /// @brief Parses all reactions
    /// @param objects A json object that should contain only valid reactions
    /// @param existing_species A list of spcecies configured for a mechanism
    /// @param existing_phases A list of phases configured for a mechanism
    /// @return A pair indicating parsing status and a Reactions struct filled with reactions configured for a mechansim
    std::pair<ConfigParseStatus, types::Reactions>
    ParseReactions(const json& objects, const std::vector<types::Species>& existing_species, const std::vector<types::Phase>& existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Reactions reactions;

      for (const auto& object : objects)
      {
        std::string type = object[validation::keys.type].get<std::string>();
        if (type == validation::keys.Arrhenius_key)
        {
          auto arrhenius_parse = ParseArrhenius(object, existing_species, existing_phases);
          status = arrhenius_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.arrhenius.push_back(arrhenius_parse.second);
        }
        else if (type == validation::keys.CondensedPhaseArrhenius_key)
        {
          auto condensed_phase_arrhenius_parse = ParseCondensedPhaseArrhenius(object, existing_species, existing_phases);
          status = condensed_phase_arrhenius_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.condensed_phase_arrhenius.push_back(condensed_phase_arrhenius_parse.second);
        }
        else if (type == validation::keys.Troe_key)
        {
          auto troe_parse = ParseTroe(object, existing_species, existing_phases);
          status = troe_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.troe.push_back(troe_parse.second);
        }
        else if (type == validation::keys.Branched_key)
        {
          auto branched_parse = ParseBranched(object, existing_species, existing_phases);
          status = branched_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.branched.push_back(branched_parse.second);
        }
        else if (type == validation::keys.Tunneling_key)
        {
          auto tunneling_parse = ParseTunneling(object, existing_species, existing_phases);
          status = tunneling_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.tunneling.push_back(tunneling_parse.second);
        }
        else if (type == validation::keys.Surface_key)
        {
          auto surface_parse = ParseSurface(object, existing_species, existing_phases);
          status = surface_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.surface.push_back(surface_parse.second);
        }
        else if (type == validation::keys.Photolysis_key)
        {
          auto photolysis_parse = ParsePhotolysis(object, existing_species, existing_phases);
          status = photolysis_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.photolysis.push_back(photolysis_parse.second);
        }
        else if (type == validation::keys.CondensedPhasePhotolysis_key)
        {
          auto condensed_phase_photolysis_parse = ParseCondensedPhasePhotolysis(object, existing_species, existing_phases);
          status = condensed_phase_photolysis_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.condensed_phase_photolysis.push_back(condensed_phase_photolysis_parse.second);
        }
        else if (type == validation::keys.Emission_key)
        {
          auto emission_parse = ParseEmission(object, existing_species, existing_phases);
          status = emission_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.emission.push_back(emission_parse.second);
        }
        else if (type == validation::keys.FirstOrderLoss_key)
        {
          auto first_order_loss_parse = ParseFirstOrderLoss(object, existing_species, existing_phases);
          status = first_order_loss_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.first_order_loss.push_back(first_order_loss_parse.second);
        }

        else if (type == validation::keys.SimpolPhaseTransfer_key)
        {
          auto simpol_phase_transfer_parse = ParseSimpolPhaseTransfer(object, existing_species, existing_phases);
          status = simpol_phase_transfer_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.simpol_phase_transfer.push_back(simpol_phase_transfer_parse.second);
        }
        else if (type == validation::keys.AqueousPhaseEquilibrium_key)
        {
          auto aqueous_equilibrium_parse = ParseAqueousEquilibrium(object, existing_species, existing_phases);
          status = aqueous_equilibrium_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.aqueous_equilibrium.push_back(aqueous_equilibrium_parse.second);
        }
        else if (type == validation::keys.WetDeposition_key)
        {
          auto wet_deposition_parse = ParseWetDeposition(object, existing_species, existing_phases);
          status = wet_deposition_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.wet_deposition.push_back(wet_deposition_parse.second);
        }
        else if (type == validation::keys.HenrysLaw_key)
        {
          auto henrys_law_parse = ParseHenrysLaw(object, existing_species, existing_phases);
          status = henrys_law_parse.first;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
          reactions.henrys_law.push_back(henrys_law_parse.second);
        }
      }

      return { status, reactions };
    }

    /// @brief Parse a mechanism
    /// @param file_path a location on the hard drive containing a mechanism
    /// @return A pair containing the parsing status and a mechanism
    std::pair<ConfigParseStatus, types::Mechanism> JsonParser::Parse(const std::string& file_path)
    {
      return JsonParser::Parse(std::filesystem::path(file_path));
    }

    /// @brief Parse a mechanism
    /// @param file_path a location on the hard drive containing a mechanism
    /// @return A pair containing the parsing status and a mechanism
    std::pair<ConfigParseStatus, types::Mechanism> JsonParser::Parse(const std::filesystem::path& file_path)
    {
      ConfigParseStatus status;

      if (!std::filesystem::exists(file_path) || std::filesystem::is_directory(file_path))
      {
        status = ConfigParseStatus::InvalidFilePath;
        std::string msg = configParseStatusToString(status);
        std::cerr << msg << std::endl;
        return { status, types::Mechanism() };
      }

      json config = json::parse(std::ifstream(file_path));

      return JsonParser::Parse(config);
    }

    /// @brief Parse a mechanism
    /// @param object A json object representing a mechanism
    /// @return A pair containing the parsing status and a mechanism
    std::pair<ConfigParseStatus, types::Mechanism> JsonParser::Parse(const nlohmann::json& object)
    {
      ConfigParseStatus status;
      types::Mechanism mechanism;

      status = ValidateSchema(object, validation::mechanism.required_keys, validation::mechanism.optional_keys);

      if (status != ConfigParseStatus::Success)
      {
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Invalid top level configuration." << std::endl;
        return { status, mechanism };
      }

      std::string version = object[validation::keys.version].get<std::string>();

      if (version != getVersionString())
      {
        status = ConfigParseStatus::InvalidVersion;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] This parser supports version " << getVersionString() << " and you requested version " << version
                  << ". Please download the appropriate version of the parser or switch to the supported format's version." << std::endl;
      }

      std::string name = object[validation::keys.name].get<std::string>();
      mechanism.name = name;

      // parse all of the species at once
      auto species_parsing = ParseSpecies(object[validation::keys.species]);

      if (species_parsing.first != ConfigParseStatus::Success)
      {
        status = species_parsing.first;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Failed to parse the species." << std::endl;
      }

      // parse all of the phases at once
      auto phases_parsing = ParsePhases(object[validation::keys.phases], species_parsing.second);

      if (phases_parsing.first != ConfigParseStatus::Success)
      {
        status = phases_parsing.first;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Failed to parse the phases." << std::endl;
      }

      // parse all of the reactions at once
      auto reactions_parsing = ParseReactions(object[validation::keys.reactions], species_parsing.second, phases_parsing.second);

      if (reactions_parsing.first != ConfigParseStatus::Success)
      {
        status = reactions_parsing.first;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Failed to parse the reactions." << std::endl;
      }

      mechanism.species = species_parsing.second;
      mechanism.phases = phases_parsing.second;
      mechanism.reactions = reactions_parsing.second;

      return { status, mechanism };
    }
  }  // namespace mechanism_configuration
}  // namespace open_atmos

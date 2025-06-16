#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors HenrysLawParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::HenrysLaw henrys_law;

      std::vector<std::string> required_keys = {
        validation::type, validation::gas_phase, validation::gas_phase_species, validation::aqueous_phase, validation::aqueous_phase_species
      };
      std::vector<std::string> optional_keys = { validation::name };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::string gas_phase = object[validation::gas_phase].as<std::string>();
        std::string gas_phase_species = object[validation::gas_phase_species].as<std::string>();
        std::string aqueous_phase = object[validation::aqueous_phase].as<std::string>();
        std::string aqueous_phase_species = object[validation::aqueous_phase_species].as<std::string>();

        if (object[validation::name])
        {
          henrys_law.name = object[validation::name].as<std::string>();
        }

        std::vector<std::string> requested_species;
        requested_species.push_back(gas_phase_species);
        requested_species.push_back(aqueous_phase_species);

        std::vector<std::string> requested_aqueous_species;
        requested_aqueous_species.push_back(aqueous_phase_species);

        if (RequiresUnknownSpecies(requested_species, existing_species))
        {
          std::string line = std::to_string(object.Mark().line + 1);
          std::string column = std::to_string(object.Mark().column + 1);
          errors.push_back({ ConfigParseStatus::ReactionRequiresUnknownSpecies, line + ":" + column + ": Reaction requires unknown species" });
        }

        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::gas_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::gas_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + gas_phase });
        }

        auto phase_it = std::find_if(
            existing_phases.begin(), existing_phases.end(), [&aqueous_phase](const types::Phase& phase) { return phase.name == aqueous_phase; });

        if (phase_it != existing_phases.end())
        {
          std::vector<std::string> aqueous_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
          if (RequiresUnknownSpecies(requested_aqueous_species, aqueous_phase_species))
          {
            std::string line = std::to_string(object.Mark().line + 1);
            std::string column = std::to_string(object.Mark().column + 1);
            errors.push_back({ ConfigParseStatus::RequestedAqueousSpeciesNotIncludedInAqueousPhase,
                               line + ":" + column + ": Requested aqueous species not included in aqueous phase" });
          }
        }
        else
        {
          std::string line = std::to_string(object[validation::aqueous_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::aqueous_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + aqueous_phase });
        }

        henrys_law.gas_phase = gas_phase;
        types::ReactionComponent gas_component;
        gas_component.species_name = gas_phase_species;
        henrys_law.gas_phase_species = gas_component;
        henrys_law.aqueous_phase = aqueous_phase;
        types::ReactionComponent aqueous_component;
        aqueous_component.species_name = aqueous_phase_species;
        henrys_law.aqueous_phase_species = aqueous_component;
        henrys_law.unknown_properties = GetComments(object);
        reactions.henrys_law.push_back(henrys_law);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

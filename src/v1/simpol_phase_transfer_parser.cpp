#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors SimpolPhaseTransferParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::SimpolPhaseTransfer simpol_phase_transfer;

      std::vector<std::string> required_keys = {
        validation::type, validation::gas_phase, validation::gas_phase_species, validation::aerosol_phase, validation::aerosol_phase_species,
        validation::B
      };
      std::vector<std::string> optional_keys = { validation::name };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::string gas_phase_species = object[validation::gas_phase_species].as<std::string>();
        std::string aerosol_phase_species = object[validation::aerosol_phase_species].as<std::string>();

        if (object[validation::name])
        {
          simpol_phase_transfer.name = object[validation::name].as<std::string>();
        }

        std::vector<std::string> requested_species{ gas_phase_species, aerosol_phase_species };
        if (RequiresUnknownSpecies(requested_species, existing_species))
        {
          std::string line = std::to_string(object.Mark().line + 1);
          std::string column = std::to_string(object.Mark().column + 1);
          errors.push_back(
              { ConfigParseStatus::ReactionRequiresUnknownSpecies, line + ":" + column + ": Reaction requires unknown species in object" });
        }

        std::string aerosol_phase = object[validation::aerosol_phase].as<std::string>();
        auto aerosol_it =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const auto& phase) { return phase.name == aerosol_phase; });
        if (aerosol_it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::aerosol_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::aerosol_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + aerosol_phase });
        }
        else
        {
          auto phase = *aerosol_it;
          auto spec_it = std::find_if(
              phase.species.begin(),
              phase.species.end(),
              [&aerosol_phase_species](const std::string& species) { return species == aerosol_phase_species; });
          if (spec_it == phase.species.end())
          {
            std::string line = std::to_string(object[validation::aerosol_phase_species].Mark().line + 1);
            std::string column = std::to_string(object[validation::aerosol_phase_species].Mark().column + 1);
            errors.push_back(
                { ConfigParseStatus::ReactionRequiresUnknownSpecies, line + ":" + column + ": Unknown species: " + aerosol_phase_species });
          }
        }

        std::string gas_phase = object[validation::gas_phase].as<std::string>();
        auto gas_it =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (gas_it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::gas_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::gas_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + gas_phase });
        }
        else
        {
          auto phase = *gas_it;
          auto spec_it = std::find_if(
              phase.species.begin(), phase.species.end(), [&gas_phase_species](const std::string& species) { return species == gas_phase_species; });
          if (spec_it == phase.species.end())
          {
            std::string line = std::to_string(object[validation::gas_phase_species].Mark().line + 1);
            std::string column = std::to_string(object[validation::gas_phase_species].Mark().column + 1);
            errors.push_back({ ConfigParseStatus::ReactionRequiresUnknownSpecies, line + ":" + column + ": Unknown species: " + gas_phase_species });
          }
        }

        if (object[validation::B] && object[validation::B].IsSequence() && object[validation::B].size() == 4)
        {
          for (size_t i = 0; i < 4; ++i)
          {
            simpol_phase_transfer.B[i] = object[validation::B][i].as<double>();
          }
        }

        simpol_phase_transfer.gas_phase = gas_phase;
        types::ReactionComponent gas_component;
        gas_component.species_name = gas_phase_species;
        simpol_phase_transfer.gas_phase_species = gas_component;
        simpol_phase_transfer.aerosol_phase = aerosol_phase;
        types::ReactionComponent aerosol_component;
        aerosol_component.species_name = aerosol_phase_species;
        simpol_phase_transfer.aerosol_phase_species = aerosol_component;
        simpol_phase_transfer.unknown_properties = GetComments(object);
        reactions.simpol_phase_transfer.push_back(simpol_phase_transfer);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

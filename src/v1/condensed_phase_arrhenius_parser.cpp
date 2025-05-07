#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors CondensedPhaseArrheniusParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::CondensedPhaseArrhenius condensed_phase_arrhenius;

      std::vector<std::string> required_keys = {
        validation::products, validation::reactants, validation::type, validation::aerosol_phase, validation::aerosol_phase_water
      };
      std::vector<std::string> optional_keys = { validation::A, validation::B,  validation::C,   validation::D,
                                                 validation::E, validation::Ea, validation::name };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        auto products = ParseReactantsOrProducts(validation::products, object);
        errors.insert(errors.end(), products.first.begin(), products.first.end());
        auto reactants = ParseReactantsOrProducts(validation::reactants, object);
        errors.insert(errors.end(), reactants.first.begin(), reactants.first.end());

        if (object[validation::A])
        {
          condensed_phase_arrhenius.A = object[validation::A].as<double>();
        }
        if (object[validation::B])
        {
          condensed_phase_arrhenius.B = object[validation::B].as<double>();
        }
        if (object[validation::C])
        {
          condensed_phase_arrhenius.C = object[validation::C].as<double>();
        }
        if (object[validation::D])
        {
          condensed_phase_arrhenius.D = object[validation::D].as<double>();
        }
        if (object[validation::E])
        {
          condensed_phase_arrhenius.E = object[validation::E].as<double>();
        }
        if (object[validation::Ea])
        {
          if (condensed_phase_arrhenius.C != 0)
          {
            std::string line = std::to_string(object[validation::Ea].Mark().line + 1);
            std::string column = std::to_string(object[validation::Ea].Mark().column + 1);
            errors.push_back({ ConfigParseStatus::MutuallyExclusiveOption, line + ":" + column + ": Cannot specify both 'C' and 'Ea'" });
          }
          condensed_phase_arrhenius.C = -1 * object[validation::Ea].as<double>() / constants::boltzmann;
        }

        if (object[validation::name])
        {
          condensed_phase_arrhenius.name = object[validation::name].as<std::string>();
        }

        std::string aerosol_phase = object[validation::aerosol_phase].as<std::string>();
        std::string aerosol_phase_water = object[validation::aerosol_phase_water].as<std::string>();

        std::vector<std::string> requested_species;
        for (const auto& spec : products.second)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants.second)
        {
          requested_species.push_back(spec.species_name);
        }
        requested_species.push_back(aerosol_phase_water);

        if (RequiresUnknownSpecies(requested_species, existing_species))
        {
          std::string line = std::to_string(object.Mark().line + 1);
          std::string column = std::to_string(object.Mark().column + 1);
          errors.push_back({ ConfigParseStatus::ReactionRequiresUnknownSpecies, line + ":" + column + ": Reaction requires unknown species" });
        }

        auto phase_it = std::find_if(
            existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const types::Phase& phase) { return phase.name == aerosol_phase; });

        if (phase_it != existing_phases.end())
        {
          std::vector<std::string> aerosol_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
          if (RequiresUnknownSpecies(requested_species, aerosol_phase_species))
          {
            std::string line = std::to_string(object.Mark().line + 1);
            std::string column = std::to_string(object.Mark().column + 1);
            errors.push_back({ ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase,
                               line + ":" + column + ": Requested aerosol species not included in aerosol phase" });
          }
        }
        else
        {
          std::string line = std::to_string(object.Mark().line + 1);
          std::string column = std::to_string(object.Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + aerosol_phase });
        }

        condensed_phase_arrhenius.aerosol_phase = aerosol_phase;
        condensed_phase_arrhenius.aerosol_phase_water = aerosol_phase_water;
        condensed_phase_arrhenius.products = products.second;
        condensed_phase_arrhenius.reactants = reactants.second;
        condensed_phase_arrhenius.unknown_properties = GetComments(object);
        reactions.condensed_phase_arrhenius.push_back(condensed_phase_arrhenius);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

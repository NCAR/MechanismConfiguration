#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors SurfaceParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::Surface surface;

      std::vector<std::string> required_keys = {
        validation::gas_phase_products, validation::gas_phase_species, validation::type, validation::gas_phase, validation::aerosol_phase
      };
      std::vector<std::string> optional_keys = { validation::name, validation::reaction_probability };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::string gas_phase_species = object[validation::gas_phase_species].as<std::string>();

        auto products = ParseReactantsOrProducts(validation::gas_phase_products, object);
        errors.insert(errors.end(), products.first.begin(), products.first.end());

        if (object[validation::reaction_probability])
        {
          surface.reaction_probability = object[validation::reaction_probability].as<double>();
        }

        if (object[validation::name])
        {
          surface.name = object[validation::name].as<std::string>();
        }

        std::vector<std::string> requested_species;
        for (const auto& spec : products.second)
        {
          requested_species.push_back(spec.species_name);
        }
        requested_species.push_back(gas_phase_species);

        if (RequiresUnknownSpecies(requested_species, existing_species))
        {
          std::string line = std::to_string(object.Mark().line + 1);
          std::string column = std::to_string(object.Mark().column + 1);
          errors.push_back(
              { ConfigParseStatus::ReactionRequiresUnknownSpecies, line + ":" + column + ": Reaction requires unknown species in object" });
        }

        std::string aerosol_phase = object[validation::aerosol_phase].as<std::string>();
        auto it =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const auto& phase) { return phase.name == aerosol_phase; });
        if (it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::aerosol_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::aerosol_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + aerosol_phase });
        }

        surface.gas_phase = object[validation::gas_phase].as<std::string>();
        surface.aerosol_phase = aerosol_phase;
        surface.gas_phase_products = products.second;
        types::ReactionComponent component;
        component.species_name = gas_phase_species;
        surface.gas_phase_species = component;
        surface.unknown_properties = GetComments(object);
        reactions.surface.push_back(surface);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

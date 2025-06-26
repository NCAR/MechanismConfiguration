#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/mechanism_parsers.hpp>
#include <mechanism_configuration/v1/reaction_parsers.hpp>
#include <mechanism_configuration/v1/reaction_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors WetDepositionParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::WetDeposition wet_deposition;

      std::vector<std::string> required_keys = { validation::aqueous_phase, validation::type };
      std::vector<std::string> optional_keys = { validation::name, validation::scaling_factor };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        if (object[validation::scaling_factor])
        {
          wet_deposition.scaling_factor = object[validation::scaling_factor].as<double>();
        }

        if (object[validation::name])
        {
          wet_deposition.name = object[validation::name].as<std::string>();
        }

        std::string aqueous_phase = object[validation::aqueous_phase].as<std::string>();

        auto it =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&aqueous_phase](const auto& phase) { return phase.name == aqueous_phase; });
        if (it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::aqueous_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::aqueous_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + aqueous_phase });
        }

        wet_deposition.aqueous_phase = aqueous_phase;
        wet_deposition.unknown_properties = GetComments(object);
        reactions.wet_deposition.push_back(wet_deposition);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration

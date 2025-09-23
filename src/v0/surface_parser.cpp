#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    Errors SurfaceParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      Errors errors;
      std::vector<std::string> required = {
        validation::TYPE, validation::GAS_PHASE_PRODUCTS, validation::GAS_PHASE_REACTANT, validation::MUSICA_NAME
      };
      std::vector<std::string> optional = { validation::PROBABILITY };

      auto validate = ValidateSchema(object, required, optional);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        types::Surface parameters;
          
        std::vector<types::ReactionComponent> reactants;
        std::vector<types::ReactionComponent> products;

        if (!object[validation::GAS_PHASE_REACTANT] || !object[validation::GAS_PHASE_REACTANT].IsScalar())
        {
          std::string line = std::to_string(object[validation::GAS_PHASE_REACTANT].Mark().line + 1);
          std::string column = std::to_string(object[validation::GAS_PHASE_REACTANT].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::InvalidObject,
                             line + ":" + column + ": error: " + std::string(validation::GAS_PHASE_REACTANT) + " is an object when it should be a string" });
        }
        else {
          std::string species_name = object[validation::GAS_PHASE_REACTANT].as<std::string>();
          parameters.gas_phase_species = { .species_name = species_name, .coefficient = 1.0 };
        }

        auto parse_error = ParseProducts(object[validation::GAS_PHASE_PRODUCTS], products);
        errors.insert(errors.end(), parse_error.begin(), parse_error.end());

        parameters.gas_phase_products = products;

        if (object[validation::PROBABILITY])
        {
          parameters.reaction_probability = object[validation::PROBABILITY].as<double>();
        }

        std::string name = "SURF." + object[validation::MUSICA_NAME].as<std::string>();
        parameters.name = name;

        mechanism->reactions.surface.push_back(parameters);
      }

      return errors;
    }
  }  // namespace v0
}  // namespace mechanism_configuration

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
        std::vector<types::ReactionComponent> reactants;
        std::vector<types::ReactionComponent> products;

        std::string species_name = object[validation::GAS_PHASE_REACTANT].as<std::string>();
        reactants.push_back({ .species_name = species_name, .coefficient = 1.0 });

        auto parse_error = ParseProducts(object[validation::GAS_PHASE_PRODUCTS], products);
        errors.insert(errors.end(), parse_error.begin(), parse_error.end());

        types::Surface parameters;

        parameters.gas_phase_species = reactants[0];
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

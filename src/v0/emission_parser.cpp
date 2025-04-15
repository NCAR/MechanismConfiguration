#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    Errors EmissionParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      Errors errors;

      std::vector<std::string> required = { validation::TYPE, validation::SPECIES, validation::MUSICA_NAME };
      std::vector<std::string> optional = { validation::SCALING_FACTOR, validation::PRODUCTS };

      auto validate = ValidateSchema(object, required, optional);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::string species = object[validation::SPECIES].as<std::string>();
        YAML::Node products_object{};
        std::vector<types::ReactionComponent> reactants;
        std::vector<types::ReactionComponent> products;
        products.push_back({ .species_name = species, .coefficient = 1.0 });
        double scaling_factor = object[validation::SCALING_FACTOR] ? object[validation::SCALING_FACTOR].as<double>() : 1.0;

        std::string name = "EMIS." + object[validation::MUSICA_NAME].as<std::string>();
        types::UserDefined user_defined = { .scaling_factor = scaling_factor, .reactants = reactants, .products = products, .name = name };
        mechanism->reactions.user_defined.push_back(user_defined);
      }

      return errors;
    }
  }  // namespace v0
}  // namespace mechanism_configuration

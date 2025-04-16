#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    Errors UserDefinedParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      Errors errors;

      std::vector<std::string> required = { validation::TYPE, validation::REACTANTS, validation::PRODUCTS, validation::MUSICA_NAME };
      std::vector<std::string> optional = { validation::SCALING_FACTOR };

      auto validate = ValidateSchema(object, required, optional);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::vector<types::ReactionComponent> reactants;
        std::vector<types::ReactionComponent> products;

        auto parse_error = ParseReactants(object[validation::REACTANTS], reactants);
        errors.insert(errors.end(), parse_error.begin(), parse_error.end());

        parse_error = ParseProducts(object[validation::PRODUCTS], products);
        errors.insert(errors.end(), parse_error.begin(), parse_error.end());

        double scaling_factor = object[validation::SCALING_FACTOR] ? object[validation::SCALING_FACTOR].as<double>() : 1.0;

        std::string name = "USER." + object[validation::MUSICA_NAME].as<std::string>();

        types::UserDefined user_defined = { .scaling_factor = scaling_factor, .reactants = reactants, .products = products, .name = name };
        mechanism->reactions.user_defined.push_back(user_defined);
      }

      return errors;
    }
  }  // namespace v0
}  // namespace mechanism_configuration

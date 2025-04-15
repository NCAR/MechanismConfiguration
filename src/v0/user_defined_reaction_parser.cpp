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

      auto required = { validation::keys.TYPE, validation::keys.REACTANTS, validation::keys.PRODUCTS, validation::keys.MUSICA_NAME };
      auto optional = { validation::keys.SCALING_FACTOR };

      auto validate = ValidateSchema(object, required, optional);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::vector<types::ReactionComponent> reactants;
        std::vector<types::ReactionComponent> products;

        auto parse_error = ParseReactants(object[validation::keys.REACTANTS], reactants);
        errors.insert(errors.end(), parse_error.begin(), parse_error.end());

        parse_error = ParseProducts(object[validation::keys.PRODUCTS], products);
        errors.insert(errors.end(), parse_error.begin(), parse_error.end());

        double scaling_factor = object[validation::keys.SCALING_FACTOR] ? object[validation::keys.SCALING_FACTOR].as<double>() : 1.0;

        std::string name = "USER." + object[validation::keys.MUSICA_NAME].as<std::string>();

        types::UserDefined user_defined = { .scaling_factor = scaling_factor, .reactants = reactants, .products = products, .name = name };
        mechanism->reactions.user_defined.push_back(user_defined);
      }

      return errors;
    }
  }  // namespace v0
}  // namespace mechanism_configuration

#include "detail/constants.hpp"
#include "detail/v0/parser.hpp"
#include "detail/v0/parser_types.hpp"
#include "detail/v0/validation.hpp"
#include "detail/validate_schema.hpp"

namespace mechanism_configuration::v0
{
  Errors PhotolysisParser(Mechanism& mechanism, const YAML::Node& object)
  {
    Errors errors;

    std::vector<std::string_view> required = {
      validation::TYPE, validation::REACTANTS, validation::PRODUCTS, validation::MUSICA_NAME
    };
    std::vector<std::string_view> optional = { validation::SCALING_FACTOR };

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

      std::string name = "PHOTO." + object[validation::MUSICA_NAME].as<std::string>();
      types::UserDefined user_defined = {
        .scaling_factor = scaling_factor, .reactants = reactants, .products = products, .name = name
      };
      mechanism.reactions.user_defined.push_back(user_defined);
    }

    return errors;
  }
}  // namespace mechanism_configuration::v0

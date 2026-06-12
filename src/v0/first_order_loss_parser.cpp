#include "detail/constants.hpp"
#include "detail/v0/parser.hpp"
#include "detail/v0/parser_types.hpp"
#include "detail/v0/validation.hpp"
#include "detail/validate_schema.hpp"

namespace mechanism_configuration::v0
{
  Errors FirstOrderLossParser(Mechanism& mechanism, const YAML::Node& object)
  {
    Errors errors;

    std::vector<std::string_view> required = { validation::TYPE, validation::SPECIES, validation::MUSICA_NAME };
    std::vector<std::string_view> optional = { validation::SCALING_FACTOR };

    auto validate = ValidateSchema(object, required, optional);
    errors.insert(errors.end(), validate.begin(), validate.end());
    if (validate.empty())
    {
      std::string species = object[validation::SPECIES].as<std::string>();
      YAML::Node products_object{};
      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;

      reactants.push_back({ .name = species, .coefficient = 1.0 });
      double scaling_factor = object[validation::SCALING_FACTOR] ? object[validation::SCALING_FACTOR].as<double>() : 1.0;

      std::string name = "LOSS." + object[validation::MUSICA_NAME].as<std::string>();
      types::UserDefined user_defined = {
        .scaling_factor = scaling_factor, .reactants = reactants, .products = products, .name = name
      };
      mechanism.reactions.user_defined.push_back(user_defined);
    }

    return errors;
  }
}  // namespace mechanism_configuration::v0

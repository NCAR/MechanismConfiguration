#include "detail/constants.hpp"
#include "detail/v0/parser.hpp"
#include "detail/v0/parser_types.hpp"
#include "detail/v0/keys.hpp"
#include "detail/check_schema.hpp"

namespace mechanism_configuration::v0
{
  Errors SurfaceParser(Mechanism& mechanism, const YAML::Node& object)
  {
    Errors errors;
    std::vector<std::string_view> required = {
      keys::TYPE, keys::GAS_PHASE_PRODUCTS, keys::GAS_PHASE_REACTANT, keys::MUSICA_NAME
    };
    std::vector<std::string_view> optional = { keys::PROBABILITY };

    auto validate = CheckSchema(object, required, optional);
    errors.insert(errors.end(), validate.begin(), validate.end());
    if (validate.empty())
    {
      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;

      std::string species_name = object[keys::GAS_PHASE_REACTANT].as<std::string>();
      reactants.push_back({ .name = species_name, .coefficient = 1.0 });

      auto parse_error = ParseProducts(object[keys::GAS_PHASE_PRODUCTS], products);
      errors.insert(errors.end(), parse_error.begin(), parse_error.end());

      types::Surface parameters;

      parameters.gas_phase_species = reactants[0];
      parameters.gas_phase_products = products;

      if (object[keys::PROBABILITY])
      {
        parameters.reaction_probability = object[keys::PROBABILITY].as<double>();
      }

      std::string name = "SURF." + object[keys::MUSICA_NAME].as<std::string>();
      parameters.name = name;

      mechanism.reactions.surface.push_back(parameters);
    }

    return errors;
  }
}  // namespace mechanism_configuration::v0

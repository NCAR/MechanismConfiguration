#include "detail/conversions.hpp"
#include "detail/v0/parser.hpp"
#include "detail/v0/parser_types.hpp"
#include "detail/v0/keys.hpp"
#include "detail/check_schema.hpp"

namespace mechanism_configuration::v0
{
  Errors ArrheniusParser(Mechanism& mechanism, const YAML::Node& object)
  {
    Errors errors;

    std::vector<std::string_view> required = { keys::TYPE, keys::REACTANTS, keys::PRODUCTS };
    std::vector<std::string_view> optional = { keys::A, keys::B,  keys::C,          keys::D,
                                          keys::E, keys::Ea, keys::MUSICA_NAME };

    auto validate = CheckSchema(object, required, optional);
    errors.insert(errors.end(), validate.begin(), validate.end());
    if (validate.empty())
    {
      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;

      auto parse_error = ParseReactants(object[keys::REACTANTS], reactants);
      errors.insert(errors.end(), parse_error.begin(), parse_error.end());

      parse_error = ParseProducts(object[keys::PRODUCTS], products);
      errors.insert(errors.end(), parse_error.begin(), parse_error.end());

      types::Arrhenius parameters;
      if (object[keys::A])
      {
        parameters.A = object[keys::A].as<double>();
      }
      int total_moles = 0;
      for (const auto& reactant : reactants)
      {
        total_moles += reactant.coefficient;
      }
      parameters.A *= std::pow(conversions::MolesM3ToMoleculesCm3, total_moles - 1);
      if (object[keys::B])
      {
        parameters.B = object[keys::B].as<double>();
      }
      if (object[keys::C])
      {
        parameters.C = object[keys::C].as<double>();
      }
      if (object[keys::D])
      {
        parameters.D = object[keys::D].as<double>();
      }
      if (object[keys::E])
      {
        parameters.E = object[keys::E].as<double>();
      }
      if (object[keys::Ea])
      {
        if (parameters.C != 0)
        {
          std::string line = std::to_string(object[keys::Ea].Mark().line + 1);
          std::string column = std::to_string(object[keys::Ea].Mark().column + 1);
          errors.push_back(
              { ErrorCode::MutuallyExclusiveOption, line + ":" + column + ": Cannot specify both 'C' and 'Ea'" });
        }
        else
        {
          // Calculate 'C' using 'Ea'
          parameters.C = -1 * object[keys::Ea].as<double>() / constants::boltzmann;
        }
      }

      parameters.reactants = reactants;
      parameters.products = products;

      mechanism.reactions.arrhenius.push_back(parameters);
    }

    return errors;
  }
}  // namespace mechanism_configuration::v0

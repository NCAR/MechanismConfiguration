#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/conversions.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    Errors BranchedParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      Errors errors;

      auto required = {
        validation::keys.TYPE, validation::keys.REACTANTS, validation::keys.ALKOXY_PRODUCTS, validation::keys.NITRATE_PRODUCTS, validation::keys.X, validation::keys.Y,
        validation::keys.A0,   validation::keys.n
      };

      auto validate = ValidateSchema(object, required, {});
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::vector<types::ReactionComponent> reactants;
        std::vector<types::ReactionComponent> alkoxy_products;
        std::vector<types::ReactionComponent> nitrate_products;

        auto parse_error = ParseReactants(object[validation::keys.REACTANTS], reactants);
        errors.insert(errors.end(), parse_error.begin(), parse_error.end());

        parse_error = ParseProducts(object[validation::keys.ALKOXY_PRODUCTS], alkoxy_products);
        errors.insert(errors.end(), parse_error.begin(), parse_error.end());

        parse_error = ParseProducts(object[validation::keys.NITRATE_PRODUCTS], nitrate_products);
        errors.insert(errors.end(), parse_error.begin(), parse_error.end());

        types::Branched parameters;
        parameters.X = object[validation::keys.X].as<double>();
        // Account for the conversion of reactant concentrations to molecules cm-3
        int total_moles = 0;
        for (const auto& reactant : reactants)
        {
          total_moles += reactant.coefficient;
        }
        parameters.X *= std::pow(conversions::MolesM3ToMoleculesCm3, total_moles - 1);
        parameters.Y = object[validation::keys.Y].as<double>();
        parameters.a0 = object[validation::keys.A0].as<double>();
        parameters.n = object[validation::keys.n].as<int>();

        parameters.reactants = reactants;
        parameters.alkoxy_products = alkoxy_products;
        parameters.nitrate_products = nitrate_products;

        mechanism->reactions.branched.push_back(parameters);
      }

      return errors;
    }
  }  // namespace v0
}  // namespace mechanism_configuration

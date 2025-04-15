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

      std::vector<std::string> required = {
        validation::TYPE, validation::REACTANTS, validation::ALKOXY_PRODUCTS, validation::NITRATE_PRODUCTS, validation::X, validation::Y,
        validation::A0,   validation::n
      };

      auto validate = ValidateSchema(object, required, {});
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::vector<types::ReactionComponent> reactants;
        std::vector<types::ReactionComponent> alkoxy_products;
        std::vector<types::ReactionComponent> nitrate_products;

        auto parse_error = ParseReactants(object[validation::REACTANTS], reactants);
        errors.insert(errors.end(), parse_error.begin(), parse_error.end());

        parse_error = ParseProducts(object[validation::ALKOXY_PRODUCTS], alkoxy_products);
        errors.insert(errors.end(), parse_error.begin(), parse_error.end());

        parse_error = ParseProducts(object[validation::NITRATE_PRODUCTS], nitrate_products);
        errors.insert(errors.end(), parse_error.begin(), parse_error.end());

        types::Branched parameters;
        parameters.X = object[validation::X].as<double>();
        // Account for the conversion of reactant concentrations to molecules cm-3
        int total_moles = 0;
        for (const auto& reactant : reactants)
        {
          total_moles += reactant.coefficient;
        }
        parameters.X *= std::pow(conversions::MolesM3ToMoleculesCm3, total_moles - 1);
        parameters.Y = object[validation::Y].as<double>();
        parameters.a0 = object[validation::A0].as<double>();
        parameters.n = object[validation::n].as<int>();

        parameters.reactants = reactants;
        parameters.alkoxy_products = alkoxy_products;
        parameters.nitrate_products = nitrate_products;

        mechanism->reactions.branched.push_back(parameters);
      }

      return errors;
    }
  }  // namespace v0
}  // namespace mechanism_configuration

#include <mechanism_configuration/conversions.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    Errors TunnelingParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      Errors errors;

      std::vector<std::string> required = { validation::TYPE, validation::REACTANTS, validation::PRODUCTS };
      std::vector<std::string> optional = { validation::A, validation::B, validation::C };

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

        types::Tunneling parameters;
        if (object[validation::A])
        {
          parameters.A = object[validation::A].as<double>();
        }
        // Account for the conversion of reactant concentrations to molecules cm-3
        int total_moles = 0;
        for (auto& reactant : reactants)
        {
          total_moles += reactant.coefficient;
        }
        parameters.A *= std::pow(conversions::MolesM3ToMoleculesCm3, total_moles - 1);
        if (object[validation::B])
        {
          parameters.B = object[validation::B].as<double>();
        }
        if (object[validation::C])
        {
          parameters.C = object[validation::C].as<double>();
        }

        parameters.reactants = reactants;
        parameters.products = products;
        mechanism->reactions.tunneling.push_back(parameters);
      }

      return errors;
    }
  }  // namespace v0
}  // namespace mechanism_configuration

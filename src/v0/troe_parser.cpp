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
    Errors TroeParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      Errors errors;

      auto required = { validation::keys.TYPE, validation::keys.REACTANTS, validation::keys.PRODUCTS };
      auto optional = { validation::keys.K0_A,   validation::keys.K0_B,   validation::keys.K0_C, validation::keys.KINF_A,
                        validation::keys.KINF_B, validation::keys.KINF_C, validation::keys.FC,   validation::keys.N };

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

        types::Troe parameters;
        if (object[validation::keys.K0_A])
        {
          parameters.k0_A = object[validation::keys.K0_A].as<double>();
        }
        // Account for the conversion of reactant concentrations (including M) to molecules cm-3
        int total_moles = 0;
        for (const auto& reactant : reactants)
        {
          total_moles += reactant.coefficient;
        }
        parameters.k0_A *= std::pow(conversions::MolesM3ToMoleculesCm3, total_moles);
        if (object[validation::keys.K0_B])
        {
          parameters.k0_B = object[validation::keys.K0_B].as<double>();
        }
        if (object[validation::keys.K0_C])
        {
          parameters.k0_C = object[validation::keys.K0_C].as<double>();
        }
        if (object[validation::keys.KINF_A])
        {
          parameters.kinf_A = object[validation::keys.KINF_A].as<double>();
        }
        // Account for terms in denominator and exponent that include [M] but not other reactants
        parameters.kinf_A *= std::pow(conversions::MolesM3ToMoleculesCm3, total_moles - 1);
        if (object[validation::keys.KINF_B])
        {
          parameters.kinf_B = object[validation::keys.KINF_B].as<double>();
        }
        if (object[validation::keys.KINF_C])
        {
          parameters.kinf_C = object[validation::keys.KINF_C].as<double>();
        }
        if (object[validation::keys.FC])
        {
          parameters.Fc = object[validation::keys.FC].as<double>();
        }
        if (object[validation::keys.N])
        {
          parameters.N = object[validation::keys.N].as<double>();
        }

        parameters.reactants = reactants;
        parameters.products = products;
        mechanism->reactions.troe.push_back(parameters);
      }

      return errors;
    }
  }  // namespace v0
}  // namespace mechanism_configuration

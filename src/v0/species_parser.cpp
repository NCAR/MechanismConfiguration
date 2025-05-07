#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    Errors ParseChemicalSpecies(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      Errors errors;
      std::vector<std::string> required = { validation::NAME, validation::TYPE };
      std::vector<std::string> optional = { validation::TRACER_TYPE, validation::ABS_TOLERANCE, validation::DIFFUSION_COEFF, validation::MOL_WEIGHT };

      auto validate = ValidateSchema(object, required, optional);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::string name = object[validation::NAME].as<std::string>();
        types::Species species;
        species.name = name;

        if (object[validation::MOL_WEIGHT])
          species.molecular_weight = object[validation::MOL_WEIGHT].as<double>();
        if (object[validation::DIFFUSION_COEFF])
          species.diffusion_coefficient = object[validation::DIFFUSION_COEFF].as<double>();
        if (object[validation::ABS_TOLERANCE])
          species.absolute_tolerance = object[validation::ABS_TOLERANCE].as<double>();
        if (object[validation::TRACER_TYPE])
          species.tracer_type = object[validation::TRACER_TYPE].as<std::string>();

        // Load remaining keys as unknown properties
        for (auto it = object.begin(); it != object.end(); ++it)
        {
          auto key = it->first.as<std::string>();
          auto value = it->second;

          if (std::find(required.begin(), required.end(), key) == required.end() &&
              std::find(optional.begin(), optional.end(), key) == optional.end())
          {
            std::string stringValue = value.as<std::string>();
            species.unknown_properties[key] = stringValue;
          }
        }
        mechanism->species.push_back(species);
      }

      return errors;
    }

    Errors ParseRelativeTolerance(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      Errors errors;
      std::vector<std::string> required = { validation::VALUE, validation::TYPE };

      auto validate = ValidateSchema(object, required, {});
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        mechanism->relative_tolerance = object[validation::VALUE].as<double>();
      }

      return errors;
    }

    Errors ParseReactants(const YAML::Node& object, std::vector<types::ReactionComponent>& reactants)
    {
      Errors errors;
      for (auto it = object.begin(); it != object.end(); ++it)
      {
        auto key = it->first.as<std::string>();
        auto value = it->second;

        auto validate = ValidateSchema(value, {}, { validation::QTY });
        errors.insert(errors.end(), validate.begin(), validate.end());
        if (validate.empty())
        {
          double qty = 1;
          if (value[validation::QTY])
            qty = value[validation::QTY].as<std::size_t>();
          types::ReactionComponent reactant = { .species_name = key, .coefficient = qty };
          reactants.push_back(reactant);
        }
      }

      return errors;
    }

    Errors ParseProducts(const YAML::Node& object, std::vector<types::ReactionComponent>& products)
    {
      Errors errors;
      for (auto it = object.begin(); it != object.end(); ++it)
      {
        auto key = it->first.as<std::string>();
        auto value = it->second;

        auto validate = ValidateSchema(value, {}, { validation::YIELD });
        errors.insert(errors.end(), validate.begin(), validate.end());
        if (validate.empty())
        {
          types::ReactionComponent product = { .species_name = key, .coefficient = 1 };
          if (value[validation::YIELD])
          {
            double yield = value[validation::YIELD].as<double>();
            product.coefficient = yield;
          }
          products.push_back(product);
        }
      }
      return errors;
    }
  }  // namespace v0
}  // namespace mechanism_configuration
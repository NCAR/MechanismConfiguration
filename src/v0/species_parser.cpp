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
      std::vector<std::string> required = { validation::keys.NAME, validation::keys.TYPE };
      std::vector<std::string> optional = { validation::keys.TRACER_TYPE, validation::keys.ABS_TOLERANCE, validation::keys.DIFFUSION_COEFF, validation::keys.MOL_WEIGHT };

      auto validate = ValidateSchema(object, required, optional);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::string name = object[validation::keys.NAME].as<std::string>();
        types::Species species;
        species.name = name;

        if (object[validation::keys.MOL_WEIGHT])
          species.molecular_weight = object[validation::keys.MOL_WEIGHT].as<double>();
        if (object[validation::keys.DIFFUSION_COEFF])
          species.diffusion_coefficient = object[validation::keys.DIFFUSION_COEFF].as<double>();
        if (object[validation::keys.ABS_TOLERANCE])
          species.absolute_tolerance = object[validation::keys.ABS_TOLERANCE].as<double>();
        if (object[validation::keys.TRACER_TYPE])
          species.tracer_type = object[validation::keys.TRACER_TYPE].as<std::string>();

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
      auto required = { validation::keys.VALUE, validation::keys.TYPE };

      auto validate = ValidateSchema(object, required, {});
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        mechanism->relative_tolerance = object[validation::keys.VALUE].as<double>();
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

        auto validate = ValidateSchema(value, {}, { validation::keys.QTY });
        errors.insert(errors.end(), validate.begin(), validate.end());
        if (validate.empty())
        {
          double qty = 1;
          if (value[validation::keys.QTY])
            qty = value[validation::keys.QTY].as<std::size_t>();
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

        auto validate = ValidateSchema(value, {}, { validation::keys.YIELD });
        errors.insert(errors.end(), validate.begin(), validate.end());
        if (validate.empty())
        {
          types::ReactionComponent product = { .species_name = key, .coefficient = 1 };
          if (value[validation::keys.YIELD])
          {
            double yield = value[validation::keys.YIELD].as<double>();
            product.coefficient = yield;
          }
          products.push_back(product);
        }
      }
      return errors;
    }
  }  // namespace v0
}  // namespace mechanism_configuration
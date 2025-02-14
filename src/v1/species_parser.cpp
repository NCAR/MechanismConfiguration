#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/v1/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    std::pair<Errors, std::vector<v1::types::Species>> ParseSpecies(const YAML::Node& objects)
    {
      Errors errors;
      std::vector<types::Species> all_species;

      for (const auto& object : objects)
      {
        types::Species species;
        auto validate = ValidateSchema(object, validation::species.required_keys, validation::species.optional_keys);
        errors.insert(errors.end(), validate.begin(), validate.end());
        if (validate.empty())
        {
          std::string name = object[validation::keys.name].as<std::string>();

          std::map<std::string, double> numerical_properties{};
          for (const auto& key : validation::species.optional_keys)
          {
            if (object[key])
            {
              double val = object[key].as<double>();
              numerical_properties[key] = val;
            }
          }

          species.name = name;
          species.optional_numerical_properties = numerical_properties;
          species.unknown_properties = GetComments(object, validation::species.required_keys, validation::species.optional_keys);

          all_species.push_back(species);
        }
      }

      if (!ContainsUniqueObjectsByName<types::Species>(all_species))
      {
        errors.push_back({ ConfigParseStatus::DuplicateSpeciesDetected, "Duplicate species detected." });
      }

      return { errors, all_species };
    }
  }
}
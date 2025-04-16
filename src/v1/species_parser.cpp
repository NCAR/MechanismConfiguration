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
        std::vector<std::string> required_keys = { validation::name };
        std::vector<std::string> optional_keys = { validation::absolute_tolerance,
                                                   validation::diffusion_coefficient,
                                                   validation::molecular_weight,
                                                   validation::henrys_law_constant_298,
                                                   validation::henrys_law_constant_exponential_factor,
                                                   validation::n_star,
                                                   validation::density,
                                                   validation::tracer_type };
        auto validate = ValidateSchema(object, required_keys, optional_keys);
        errors.insert(errors.end(), validate.begin(), validate.end());
        if (validate.empty())
        {
          std::string name = object[validation::name].as<std::string>();
          species.name = name;

          if (object[validation::tracer_type])
            species.tracer_type = object[validation::tracer_type].as<std::string>();

          if (object[validation::absolute_tolerance])
            species.absolute_tolerance = object[validation::absolute_tolerance].as<double>();
          if (object[validation::diffusion_coefficient])
            species.diffusion_coefficient = object[validation::diffusion_coefficient].as<double>();
          if (object[validation::molecular_weight])
            species.molecular_weight = object[validation::molecular_weight].as<double>();
          if (object[validation::henrys_law_constant_298])
            species.henrys_law_constant_298 = object[validation::henrys_law_constant_298].as<double>();
          if (object[validation::henrys_law_constant_exponential_factor])
            species.henrys_law_constant_exponential_factor = object[validation::henrys_law_constant_exponential_factor].as<double>();
          if (object[validation::n_star])
            species.n_star = object[validation::n_star].as<double>();
          if (object[validation::density])
            species.density = object[validation::density].as<double>();

          species.unknown_properties = GetComments(object);

          all_species.push_back(species);
        }
      }

      if (!ContainsUniqueObjectsByName<types::Species>(all_species))
      {
        errors.push_back({ ConfigParseStatus::DuplicateSpeciesDetected, "Duplicate species detected." });
      }

      return { errors, all_species };
    }
  }  // namespace v1
}  // namespace mechanism_configuration

#include <mechanism_configuration/v1/model_parsers.hpp>
#include <mechanism_configuration/v1/model_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors GasModelParser::parse(
        const YAML::Node& object,
        const std::vector<types::Phase>& existing_phases,
        types::Models& models)
    {
      Errors errors;
      types::GasModel gas_model;

      std::vector<std::string> required_keys = { validation::type, validation::phase };
      std::vector<std::string> optional_keys = { validation::name };

      auto has_error = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), has_error.begin(), has_error.end());
      if (has_error.empty())
      {
        gas_model.type = object[validation::type].as<std::string>();
        gas_model.phase = object[validation::phase].as<std::string>();

        if (object[validation::name])
        {
          gas_model.name = object[validation::name].as<std::string>();
        }

        gas_model.unknown_properties = GetComments(object);
        models.push_back(gas_model);
      }

      return errors;
    }

  }  // namespace v1
}  // namespace mechanism_configuration

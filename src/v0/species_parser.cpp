// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/check_schema.hpp"
#include "detail/v0/keys.hpp"
#include "detail/v0/parser_types.hpp"

#include <mechanism_configuration/errors.hpp>

namespace mechanism_configuration::v0
{
  Errors ParseChemicalSpecies(Mechanism& mechanism, const YAML::Node& object)
  {
    Errors errors;
    std::vector<std::string_view> required = { keys::NAME, keys::TYPE };
    std::vector<std::string_view> optional = {
      keys::TRACER_TYPE, keys::ABS_TOLERANCE, keys::DIFFUSION_COEFF, keys::MOL_WEIGHT
    };

    auto validate = CheckSchema(object, required, optional);
    errors.insert(errors.end(), validate.begin(), validate.end());
    if (validate.empty())
    {
      std::string name = object[keys::NAME].as<std::string>();
      types::Species species;
      species.name = name;

      if (object[keys::MOL_WEIGHT])
        species.molecular_weight = object[keys::MOL_WEIGHT].as<double>();
      if (object[keys::DIFFUSION_COEFF])
        species.diffusion_coefficient = object[keys::DIFFUSION_COEFF].as<double>();
      if (object[keys::ABS_TOLERANCE])
        species.absolute_tolerance = object[keys::ABS_TOLERANCE].as<double>();
      if (object[keys::TRACER_TYPE])
        species.tracer_type = object[keys::TRACER_TYPE].as<std::string>();

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
      mechanism.species.push_back(species);
    }

    return errors;
  }

  Errors ParseRelativeTolerance(Mechanism& mechanism, const YAML::Node& object)
  {
    Errors errors;
    std::vector<std::string_view> required = { keys::VALUE, keys::TYPE };

    auto validate = CheckSchema(object, required, {});
    errors.insert(errors.end(), validate.begin(), validate.end());
    if (validate.empty())
    {
      mechanism.relative_tolerance = object[keys::VALUE].as<double>();
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

      auto validate = CheckSchema(value, {}, { keys::QTY });
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        double qty = 1;
        if (value[keys::QTY])
          qty = value[keys::QTY].as<std::size_t>();
        types::ReactionComponent reactant = { .name = key, .coefficient = qty };
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

      auto validate = CheckSchema(value, {}, { keys::YIELD });
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        types::ReactionComponent product = { .name = key, .coefficient = 1 };
        if (value[keys::YIELD])
        {
          double yield = value[keys::YIELD].as<double>();
          product.coefficient = yield;
        }
        products.push_back(product);
      }
    }
    return errors;
  }
}  // namespace mechanism_configuration::v0
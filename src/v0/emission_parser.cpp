// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/constants.hpp"
#include "detail/schema.hpp"
#include "detail/v0/keys.hpp"
#include "detail/v0/parser.hpp"
#include "detail/v0/parser_types.hpp"

namespace mechanism_configuration::v0
{
  Errors EmissionParser(Mechanism& mechanism, const YAML::Node& object)
  {
    Errors errors;

    std::vector<std::string_view> required = { keys::TYPE, keys::SPECIES, keys::MUSICA_NAME };
    std::vector<std::string_view> optional = { keys::SCALING_FACTOR, keys::PRODUCTS };

    auto validate = CheckSchema(object, required, optional);
    errors.insert(errors.end(), validate.begin(), validate.end());
    if (validate.empty())
    {
      std::string species = object[keys::SPECIES].as<std::string>();
      YAML::Node products_object{};
      std::vector<types::ReactionComponent> products;
      products.push_back({ .name = species, .coefficient = 1.0 });
      double scaling_factor = object[keys::SCALING_FACTOR] ? object[keys::SCALING_FACTOR].as<double>() : 1.0;

      std::string name = object[keys::MUSICA_NAME].as<std::string>();
      types::Emission user_defined = { .scaling_factor = scaling_factor, .products = products, .name = name };
      mechanism.reactions.emission.push_back(user_defined);
    }

    return errors;
  }
}  // namespace mechanism_configuration::v0

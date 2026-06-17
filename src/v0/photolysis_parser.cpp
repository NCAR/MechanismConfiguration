// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/check_schema.hpp"
#include "detail/constants.hpp"
#include "detail/v0/keys.hpp"
#include "detail/v0/parser.hpp"
#include "detail/v0/parser_types.hpp"

namespace mechanism_configuration::v0
{
  Errors PhotolysisParser(Mechanism& mechanism, const YAML::Node& object)
  {
    Errors errors;

    std::vector<std::string_view> required = { keys::TYPE, keys::REACTANTS, keys::PRODUCTS, keys::MUSICA_NAME };
    std::vector<std::string_view> optional = { keys::SCALING_FACTOR };

    auto validate = CheckSchema(object, required, optional);
    errors.insert(errors.end(), validate.begin(), validate.end());
    if (validate.empty())
    {
      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;

      auto parse_error = ParseReactants(object[keys::REACTANTS], reactants);
      errors.insert(errors.end(), parse_error.begin(), parse_error.end());

      parse_error = ParseProducts(object[keys::PRODUCTS], products);
      errors.insert(errors.end(), parse_error.begin(), parse_error.end());

      double scaling_factor = object[keys::SCALING_FACTOR] ? object[keys::SCALING_FACTOR].as<double>() : 1.0;

      std::string name = "PHOTO." + object[keys::MUSICA_NAME].as<std::string>();
      types::UserDefined user_defined = {
        .scaling_factor = scaling_factor, .reactants = reactants, .products = products, .name = name
      };
      mechanism.reactions.user_defined.push_back(user_defined);
    }

    return errors;
  }
}  // namespace mechanism_configuration::v0

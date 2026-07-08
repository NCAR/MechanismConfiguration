// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/reactions/schema.hpp"

#include "detail/error_format.hpp"
#include "detail/schema.hpp"
#include "detail/v1/keys.hpp"
#include "detail/v1/reactions/parsers.hpp"
#include "detail/v1/utils.hpp"

#include <mechanism_configuration/errors.hpp>

#include <string>
#include <vector>

namespace mechanism_configuration::v1
{
  Errors CheckReactantsOrProductsSchema(const YAML::Node& list)
  {
    const std::vector<std::string_view> required_keys = {};
    const std::vector<std::string_view> optional_keys = { keys::coefficient };
    // A component's species reference may use the canonical `name` or the legacy
    // `species name` alias, but exactly one of them.
    const std::vector<std::vector<std::string_view>> exactly_one_of = { { keys::name, keys::species_name } };

    Errors errors;

    for (const auto& object : list)
    {
      auto schema_errors = CheckSchema(object, required_keys, optional_keys, exactly_one_of);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }
    }

    return errors;
  }

  Errors CheckReactionsSchema(
      const YAML::Node& reactions_list,
      const std::vector<types::Species>& existing_species,
      const std::vector<types::Phase>& existing_phases)
  {
    Errors errors;

    auto& parsers = GetReactionParserMap();

    std::vector<std::pair<YAML::Node, IReactionParser*>> valid_reactions;

    for (const auto& object : reactions_list)
    {
      if (!object[keys::type])
      {
        ErrorLocation error_location{ object.Mark().line, object.Mark().column };
        std::string message = mc_fmt::format("{} error: Missing 'type' object in reaction.", error_location);
        errors.push_back({ ErrorCode::RequiredKeyNotFound, message });
        continue;
      }

      std::string type = object[keys::type].as<std::string>();

      auto it = parsers.find(type);
      if (it == parsers.end())
      {
        const auto& node = object[keys::type];
        ErrorLocation error_location{ node.Mark().line, node.Mark().column };

        std::string message = mc_fmt::format("{} error: Unknown reaction type '{}' found.", error_location, type);

        errors.push_back({ ErrorCode::UnknownType, message });

        continue;
      }
      valid_reactions.emplace_back(object, it->second.get());
    }

    if (!errors.empty())
      return errors;

    for (const auto& [reaction_node, parser] : valid_reactions)
    {
      auto schema_errors = parser->CheckSchema(reaction_node, existing_species, existing_phases);
      if (!schema_errors.empty())
      {
        errors.insert(errors.end(), schema_errors.begin(), schema_errors.end());
      }
    }

    return errors;
  }

}  // namespace mechanism_configuration::v1

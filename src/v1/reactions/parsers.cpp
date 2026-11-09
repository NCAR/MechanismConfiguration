// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v1/reactions/parsers.hpp"

#include "detail/json_pointer.hpp"
#include "detail/v1/keys.hpp"
#include "detail/v1/utils.hpp"

#include <mechanism_configuration/errors.hpp>

#include <stdexcept>

namespace mechanism_configuration::v1
{
  std::vector<types::ReactionComponent> ParseReactionComponents(const YAML::Node& object, std::string_view key)
  {
    std::vector<types::ReactionComponent> component_list;
    for (const auto& elem : AsSequence(object[key]))
    {
      types::ReactionComponent component;
      component.name = GetComponentName(elem);

      // A bare-string component carries only its name; coefficients/comments
      // are only present on the object form.
      if (!elem.IsScalar())
      {
        component.unknown_properties = GetComments(elem);
        if (elem[keys::coefficient])
        {
          component.coefficient = elem[keys::coefficient].as<double>();
        }
      }

      component_list.emplace_back(std::move(component));
    }

    return component_list;
  };

  types::ReactionComponent ParseReactionComponent(const YAML::Node& object, std::string_view key)
  {
    auto reaction_components = ParseReactionComponents(object, key);

    if (reaction_components.empty())
    {
      return types::ReactionComponent();
    }

    return std::move(reaction_components.front());
  };

  types::Reactions ParseReactions(const YAML::Node& objects, LeftOutItems& left_out)
  {
    auto& parsers = GetReactionParserMap();
    types::Reactions reactions;

    std::size_t index = 0;
    for (const auto& object : objects)
    {
      BuildItem(
          object,
          RootPath(keys::reactions, index++),
          "reaction",
          left_out,
          [&]
          {
            const auto type = object[keys::type].as<std::string>();
            auto it = parsers.find(type);
            if (it != parsers.end())
              it->second->Parse(object, reactions);
            else
              throw std::runtime_error("no parser exists for the type '" + type + "'");
          });
    }

    return reactions;
  }

}  // namespace mechanism_configuration::v1

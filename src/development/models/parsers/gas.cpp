// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include <mechanism_configuration/development/model_parsers.hpp>
#include <mechanism_configuration/development/model_types.hpp>
#include <mechanism_configuration/development/utils.hpp>

#include <utility>

namespace mechanism_configuration
{
  namespace development
  {
    void GasModelParser::Parse(const YAML::Node& object, types::Models& models)
    {
      types::GasModel model;

      model.type = object[validation::type].as<std::string>();
      model.phase = object[validation::phase].as<std::string>();
      model.unknown_properties = GetComments(object);

      if (object[validation::name])
      {
        model.name = object[validation::name].as<std::string>();
      }

      models.gas_model = std::move(model);
    }

  }  // namespace development
}  // namespace mechanism_configuration

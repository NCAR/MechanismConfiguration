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
    void ModalModelParser::Parse(const YAML::Node& object, types::Models& models)
    {
      types::ModalModel model;

      model.type = object[validation::type].as<std::string>();
      model.unknown_properties = GetComments(object);

      for (const auto& mode_object : object[validation::modes])
      {
        types::Mode mode;
        mode.name = mode_object[validation::name].as<std::string>();
        mode.geometric_mean_diameter = mode_object[validation::geometric_mean_diameter].as<double>();
        mode.geometric_standard_deviation = mode_object[validation::geometric_standard_deviation].as<double>();
        mode.phase = mode_object[validation::phase].as<std::string>();
        mode.unknown_properties = GetComments(mode_object);
        model.modes.emplace_back(std::move(mode));
      }

      if (object[validation::name])
      {
        model.name = object[validation::name].as<std::string>();
      }

      models.modal_model = std::move(model);
    }

  }  // namespace development
}  // namespace mechanism_configuration

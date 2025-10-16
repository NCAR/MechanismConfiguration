// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/development/model_types.hpp>
#include <mechanism_configuration/development/types.hpp>
#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

#include <vector>

namespace mechanism_configuration
{
  namespace development
  {
    /// @brief Abstract interface for model parsers
    class IModelParser
    {
     public:
      /// @brief Parses a YAML node representing a model and populate the models object
      /// @param object The YAML node containing model information
      /// @param existing_phases A list of previously defined phases
      /// @param models The container to which the parsed models will be added
      /// @return A list of any parsing errors encountered
      virtual Errors
      parse(const YAML::Node& object, const std::vector<types::Phase>& existing_phases, types::Models& models) = 0;

      /// @brief Destructor
      virtual ~IModelParser() = default;
    };

    /// @brief Parser for gas-phase models
    class GasModelParser : public IModelParser
    {
     public:
      /// @brief Parses a YAML node containing gas-phase model information
      Errors parse(const YAML::Node& object, const std::vector<types::Phase>& existing_phases, types::Models& models)
          override;
    };

    /// @brief Parser for modal aerosol models
    class ModalModelParser : public IModelParser
    {
     public:
      /// @brief Parses a YAML node containing modal aerosol model information
      Errors parse(const YAML::Node& object, const std::vector<types::Phase>& existing_phases, types::Models& models)
          override;
    };

  }  // namespace development
}  // namespace mechanism_configuration
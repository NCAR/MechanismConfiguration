// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/development/model_types.hpp>
#include <mechanism_configuration/development/types.hpp>
#include <mechanism_configuration/development/validation.hpp>
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
      /// @brief Validates a YAML node representing a model
      /// @param object The YAML node containing model information
      /// @param existing_phases A list of chemical phases
      /// @return A list of any validation errors encountered
      virtual Errors Validate(const YAML::Node& object, const std::vector<types::Phase>& existing_phases) = 0;

      /// @brief Parses a YAML node representing a model and populate the models object
      /// @param object The YAML node containing model information
      /// @param models The container to which the parsed models will be added
      virtual void Parse(const YAML::Node& object, types::Models& models) = 0;

      /// @brief Destructor
      virtual ~IModelParser() = default;
    };

    /// @brief Parser for gas-phase models
    class GasModelParser : public IModelParser
    {
     public:
      Errors Validate(const YAML::Node& object, const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Models& models) override;
    };

    /// @brief Parser for modal aerosol models
    class ModalModelParser : public IModelParser
    {
     public:
      Errors Validate(const YAML::Node& object, const std::vector<types::Phase>& existing_phases) override;

      void Parse(const YAML::Node& object, types::Models& models) override;
    };

    /// @brief Returns a static map of model type keys to their parser instances
    inline std::map<std::string, std::unique_ptr<IModelParser>>& GetModelParserMap()
    {
      static std::map<std::string, std::unique_ptr<IModelParser>> model_parsers = []
      {
        std::map<std::string, std::unique_ptr<IModelParser>> map;
        map[validation::GasModel_key] = std::make_unique<GasModelParser>();
        map[validation::ModalModel_key] = std::make_unique<ModalModelParser>();
        return map;
      }();

      return model_parsers;
    }

  }  // namespace development
}  // namespace mechanism_configuration
// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/emissions/v1/types.hpp>
#include <mechanism_configuration/errors.hpp>

#include <yaml-cpp/yaml.h>

#include <expected>
#include <filesystem>
#include <string>

namespace mechanism_configuration::emissions::v1
{
  class Parser
  {
   public:
    Parser() = default;

    /// @brief Parse an emissions configuration from a file.
    std::expected<types::EmissionsConfig, Errors> Parse(const std::filesystem::path& config_path);

    /// @brief Parse an emissions configuration from an in-memory YAML string.
    std::expected<types::EmissionsConfig, Errors> Parse(const std::string& content);

   private:
    std::string config_path_;

    std::expected<types::EmissionsConfig, Errors> ValidateAndBuild(const YAML::Node& object);

    std::string ExpandEnvVars(const std::string& input, Errors& errors) const;
  };

}  // namespace mechanism_configuration::emissions::v1

// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/parser_result.hpp>
#include <mechanism_configuration/v1/mechanism.hpp>

#include <yaml-cpp/yaml.h>

#include <filesystem>

namespace mechanism_configuration
{
  namespace v1
  {
    class Parser
    {
     public:
      ParserResult<types::Mechanism> Parse(const std::filesystem::path& config_path);
      ParserResult<types::Mechanism> ParseFromString(const std::string& content);
      ParserResult<types::Mechanism> ParseFromNode(const YAML::Node& object);
    };
  }  // namespace v1
}  // namespace mechanism_configuration

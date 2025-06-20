// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/parser_result.hpp>
#include <mechanism_configuration/v1/types.hpp>

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
    };
  }  // namespace v1
}  // namespace mechanism_configuration

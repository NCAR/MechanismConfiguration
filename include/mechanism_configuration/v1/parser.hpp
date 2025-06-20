// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/parser_result.hpp>

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

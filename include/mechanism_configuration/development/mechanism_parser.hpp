// Copyright (C) 2023–2025 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/development/mechanism.hpp>
#include <mechanism_configuration/parser_result.hpp>

#include <filesystem>

namespace mechanism_configuration
{
  namespace development
  {
    class Parser
    {
     public:
      ParserResult<types::Mechanism> Parse(const std::filesystem::path& config_path);
    };
  }  // namespace development
}  // namespace mechanism_configuration

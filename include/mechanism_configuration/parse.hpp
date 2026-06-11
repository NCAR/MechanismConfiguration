// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/mechanism.hpp>

#include <filesystem>
#include <expected>

namespace mechanism_configuration
{
  std::expected<Mechanism, Errors> parse(const std::filesystem::path& config_path);
  std::expected<Mechanism, Errors> parse(const std::filesystem::path& config_path);
}  // namespace mechanism_configuration
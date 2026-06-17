// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/mechanism.hpp>

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <expected>

namespace mechanism_configuration::v0
{
  class Parser
  {
    const std::string DEFAULT_CONFIG_FILE_JSON = "config.json";
    const std::string DEFAULT_CONFIG_FILE_YAML = "config.yaml";
    const std::string CAMP_FILES = "camp-files";
    const std::string CAMP_DATA = "camp-data";
    const std::string TYPE = "type";

    Errors GetCampFiles(const std::filesystem::path& config_path, std::vector<std::filesystem::path>& camp_files);

   public:
    std::expected<Mechanism, Errors> Parse(const std::filesystem::path& config_path);
  };
}  // namespace mechanism_configuration::v0

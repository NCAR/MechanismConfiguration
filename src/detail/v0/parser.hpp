// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>
#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/parse.hpp>

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>

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
    /// @brief Parses a v0 configuration. Each reaction and species is added only when its checks
    ///        pass, so the mechanism has everything that the parser can build.
    /// @return The mechanism and all the errors. The mechanism is empty only when the parser
    ///         cannot find the CAMP files.
    ParseResult Parse(const std::filesystem::path& config_path);
  };
}  // namespace mechanism_configuration::v0

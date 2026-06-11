// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/mechanism.hpp>

#include <filesystem>
#include <expected>

namespace YAML
{
  class Node;
}  // namespace YAML

namespace mechanism_configuration
{
  namespace v1
  {
    class Parser
    {
     private:
      enum class EntityFormat
      {
        FileList,  // { "files": [...] }
        Inline,    // [ { "name": ... }, ... ]
        Invalid,
      };

      EntityFormat GetEntityFormat(const YAML::Node& node);

      std::expected<Mechanism, Errors> ParseFromFileConfig(
          const YAML::Node& object,
          const std::filesystem::path& config_path,
          EntityFormat spc_format,
          EntityFormat phs_format,
          EntityFormat rxn_format);

     public:
      std::expected<Mechanism, Errors> Parse(const std::filesystem::path& config_path);
      std::expected<Mechanism, Errors> ParseFromString(const std::string& content);
      std::expected<Mechanism, Errors> ParseFromNode(const YAML::Node& object);
    };
  }  // namespace v1
}  // namespace mechanism_configuration

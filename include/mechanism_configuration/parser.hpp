#pragma once

#include <mechanism_configuration/parser_result.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v1/parser.hpp>

#include <filesystem>
#include <memory>
#include <vector>

namespace mechanism_configuration
{
  class UniversalParser
  {
   public:
    ParserResult<GlobalMechanism> Parse(const std::filesystem::path& config_path)
    {
      ParserResult<GlobalMechanism> result;
      if (!std::filesystem::exists(config_path))
      {
        result.errors.push_back({ ConfigParseStatus::FileNotFound, "File not found" });
        return result;
      }

      v1::Parser v1_parser;
      auto v1_result = v1_parser.Parse(config_path);

      if (v1_result)
      {
        result.mechanism = std::move(v1_result.mechanism);
        return result;
      }

      v0::Parser v0_parser;
      auto v0_result = v0_parser.Parse(config_path);

      if (v0_result)
      {
        result.mechanism = std::move(v0_result.mechanism);
        return result;
      }

      result.errors = v1_result.errors;
      result.errors.insert(result.errors.end(), v0_result.errors.begin(), v0_result.errors.end());
      return result;
    }
  };
}  // namespace mechanism_configuration

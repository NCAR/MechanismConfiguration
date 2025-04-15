// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include <functional>
#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/conversions.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {

    using ParserMap = std::map<std::string, std::function<Errors(std::unique_ptr<types::Mechanism>&, const YAML::Node&)>>;

    Errors run_parsers(const ParserMap& parsers, std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      Errors errors;
      for (const auto& element : object)
      {
        std::string type = element[validation::TYPE].as<std::string>();
        auto it = parsers.find(type);
        if (it != parsers.end())
        {
          auto parse_errors = it->second(mechanism, element);
          errors.insert(errors.end(), parse_errors.begin(), parse_errors.end());
        }
        else
        {
          const std::string& msg = "Unknown type: " + type;
          throw std::runtime_error(msg);
        }
      }
      return errors;
    }

    Errors ParseMechanism(const ParserMap& parsers, std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      std::vector<std::string> required = { validation::NAME, validation::REACTIONS, validation::TYPE };

      Errors errors;
      auto validate = ValidateSchema(object, required, {});
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        mechanism->name = object[validation::NAME].as<std::string>();
        auto parse_errors = run_parsers(parsers, mechanism, object[validation::REACTIONS]);
        errors.insert(errors.end(), parse_errors.begin(), parse_errors.end());
      }

      return errors;
    }

    Errors Parser::GetCampFiles(const std::filesystem::path& config_path, std::vector<std::filesystem::path>& camp_files)
    {
      Errors errors;
      // Look for CAMP config path
      if (!std::filesystem::exists(config_path))
      {
        errors.push_back({ ConfigParseStatus::FileNotFound, "File not found" });
        return errors;
      }

      std::filesystem::path config_dir;
      std::filesystem::path config_file;

      if (std::filesystem::is_directory(config_path))
      {
        // If config path is a directory, use default config file name
        config_dir = config_path;
        if (std::filesystem::exists(config_dir / DEFAULT_CONFIG_FILE_YAML))
        {
          config_file = config_dir / DEFAULT_CONFIG_FILE_YAML;
        }
        else
        {
          config_file = config_dir / DEFAULT_CONFIG_FILE_JSON;
        }
      }
      else
      {
        // Extract configuration dir from configuration file path
        config_dir = config_path.parent_path();
        config_file = config_path;
      }

      // Load the CAMP file list YAML
      YAML::Node camp_data = YAML::LoadFile(config_file.string());
      if (!camp_data[CAMP_FILES])
      {
        std::string msg = "Required key not found: " + CAMP_FILES;
        errors.push_back({ ConfigParseStatus::RequiredKeyNotFound, msg });
        return errors;
      }

      // Build a list of individual CAMP config files
      for (const auto& element : camp_data[CAMP_FILES])
      {
        std::filesystem::path camp_file = config_dir / element.as<std::string>();
        if (!std::filesystem::exists(camp_file))
        {
          errors.push_back({ ConfigParseStatus::FileNotFound, "File not found: " + camp_file.string() });
        }
        else
        {
          camp_files.push_back(camp_file);
        }
      }

      return errors;
    }

    ParserResult<types::Mechanism> Parser::Parse(const std::filesystem::path& config_path)
    {
      ConfigParseStatus status;

      ParserResult<types::Mechanism> result;
      result.mechanism = std::make_unique<types::Mechanism>();

      std::vector<std::filesystem::path> camp_files;
      auto errors = GetCampFiles(config_path, camp_files);

      if (!errors.empty())
      {
        result.errors = errors;
        return result;
      }

      ParserMap parsers;

      std::function<Errors(std::unique_ptr<types::Mechanism>&, const YAML::Node&)> ParseMechanismArray =
          [&](std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object) { return ParseMechanism(parsers, mechanism, object); };

      parsers["CHEM_SPEC"] = ParseChemicalSpecies;
      parsers["RELATIVE_TOLERANCE"] = ParseRelativeTolerance;
      parsers["PHOTOLYSIS"] = PhotolysisParser;
      parsers["EMISSION"] = EmissionParser;
      parsers["FIRST_ORDER_LOSS"] = FirstOrderLossParser;
      parsers["ARRHENIUS"] = ArrheniusParser;
      parsers["TROE"] = TroeParser;
      parsers["TERNARY_CHEMICAL_ACTIVATION"] = TernaryChemicalActivationParser;
      parsers["BRANCHED"] = BranchedParser;
      parsers["WENNBERG_NO_RO2"] = BranchedParser;
      parsers["TUNNELING"] = TunnelingParser;
      parsers["WENNBERG_TUNNELING"] = TunnelingParser;
      parsers["SURFACE"] = SurfaceParser;
      parsers["USER_DEFINED"] = UserDefinedParser;
      parsers["MECHANISM"] = ParseMechanismArray;

      for (const auto& camp_file : camp_files)
      {
        YAML::Node config_subset = YAML::LoadFile(camp_file.string());

        auto parse_errors = run_parsers(parsers, result.mechanism, config_subset[CAMP_DATA]);
        // prepend the file name to the error messages
        for (auto& error : parse_errors)
        {
          error.second = camp_file.string() + ":" + error.second;
        }
        result.errors.insert(result.errors.end(), parse_errors.begin(), parse_errors.end());
      }

      // all species in version 0 are in the gas phase
      types::Phase gas_phase;
      gas_phase.name = "GAS";
      for (auto& species : result.mechanism->species)
      {
        gas_phase.species.push_back(species.name);
      }

      result.mechanism->version = Version(0, 0, 0);

      return result;
    }
  }  // namespace v0
}  // namespace mechanism_configuration
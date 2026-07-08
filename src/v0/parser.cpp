// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/v0/parser.hpp"

#include "detail/constants.hpp"
#include "detail/conversions.hpp"
#include "detail/schema.hpp"
#include "detail/v0/keys.hpp"
#include "detail/v0/parser_types.hpp"

#include <yaml-cpp/yaml.h>

#include <functional>

namespace mechanism_configuration::v0
{
  using ParserMap = std::map<std::string, std::function<Errors(Mechanism&, const YAML::Node&)>>;

  Errors run_parsers(const ParserMap& parsers, Mechanism& mechanism, const YAML::Node& object)
  {
    Errors errors;
    for (const auto& element : object)
    {
      std::string type = element[keys::TYPE].as<std::string>();
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

  Errors ParseMechanism(const ParserMap& parsers, Mechanism& mechanism, const YAML::Node& object)
  {
    std::vector<std::string_view> required = { keys::NAME, keys::REACTIONS, keys::TYPE };

    Errors errors;
    auto validate = CheckSchema(object, required, {});
    errors.insert(errors.end(), validate.begin(), validate.end());
    if (validate.empty())
    {
      mechanism.name = object[keys::NAME].as<std::string>();
      auto parse_errors = run_parsers(parsers, mechanism, object[keys::REACTIONS]);
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
      errors.push_back({ ErrorCode::FileNotFound, "File not found" });
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
      errors.push_back({ ErrorCode::RequiredKeyNotFound, msg });
      return errors;
    }

    // Build a list of individual CAMP config files
    for (const auto& element : camp_data[CAMP_FILES])
    {
      std::filesystem::path camp_file = config_dir / element.as<std::string>();
      if (!std::filesystem::exists(camp_file))
      {
        errors.push_back({ ErrorCode::FileNotFound, "File not found: " + camp_file.string() });
      }
      else
      {
        camp_files.push_back(camp_file);
      }
    }

    return errors;
  }

  std::expected<Mechanism, Errors> Parser::Parse(const std::filesystem::path& config_path)
  {
    Errors errors;
    auto mechanism = Mechanism();

    std::vector<std::filesystem::path> camp_files;
    auto file_errors = GetCampFiles(config_path, camp_files);
    errors.insert(errors.end(), file_errors.begin(), file_errors.end());

    // Only attempt to parse the mechanism if we were able to gather the CAMP files.
    if (errors.empty())
    {
      ParserMap parsers;

      std::function<Errors(Mechanism&, const YAML::Node&)> ParseMechanismArray =
          [&](Mechanism& mechanism, const YAML::Node& object) { return ParseMechanism(parsers, mechanism, object); };

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
        // Parse each file independently so one malformed file does not abort the rest.
        try
        {
          YAML::Node config_subset = YAML::LoadFile(camp_file.string());

          auto parse_errors = run_parsers(parsers, mechanism, config_subset[CAMP_DATA]);
          // prepend the file name to the error messages
          for (auto& error : parse_errors)
          {
            error.second = camp_file.string() + ":" + error.second;
          }
          errors.insert(errors.end(), parse_errors.begin(), parse_errors.end());
        }
        catch (const std::exception& e)
        {
          errors.push_back({ ErrorCode::UnexpectedError, camp_file.string() + ":" + e.what() });
        }
      }

      // all species in version 0 are in the gas phase
      types::Phase gas_phase;
      gas_phase.name = "gas";
      for (auto& species : mechanism.species)
      {
        types::PhaseSpecies phase_species;
        phase_species.name = species.name;
        gas_phase.species.push_back(phase_species);
      }
      mechanism.phases.push_back(gas_phase);

      mechanism.version = Version(0, 0, 0);
    }

    std::expected<Mechanism, Errors> result;
    if (!errors.empty())
    {
      result = std::unexpected(std::move(errors));
    }
    else
    {
      result = mechanism;
    }
    return result;
  }
}  // namespace mechanism_configuration::v0
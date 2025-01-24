// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/v0/parser.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    bool ValidateSchema(const YAML::Node& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys)
    {
      YAML::Emitter out;
      out << object;  // Serialize the object to a string

      // standard keys are:
      // those in required keys
      // those in optional keys
      // starting with __
      // anything else is reported as an error so that typos are caught, specifically for optional keys
      if (object && object.size() > 0 && object.begin()->second.IsNull())
      {
        return false;
      }

      std::vector<std::string> sorted_object_keys;
      for (auto it = object.begin(); it != object.end(); ++it)
      {
        std::string key = it->first.as<std::string>();
        sorted_object_keys.push_back(key);
      }

      auto sorted_required_keys = required_keys;
      auto sorted_optional_keys = optional_keys;
      std::sort(sorted_object_keys.begin(), sorted_object_keys.end());
      std::sort(sorted_required_keys.begin(), sorted_required_keys.end());
      std::sort(sorted_optional_keys.begin(), sorted_optional_keys.end());

      // get the difference between the object keys and those required
      // what's left should be the optional keys and valid comments
      std::vector<std::string> difference;
      std::set_difference(
          sorted_object_keys.begin(),
          sorted_object_keys.end(),
          sorted_required_keys.begin(),
          sorted_required_keys.end(),
          std::back_inserter(difference));

      // check that the number of keys remaining is exactly equal to the expected number of required keys
      if (difference.size() != (sorted_object_keys.size() - required_keys.size()))
      {
        std::vector<std::string> missing_keys;
        std::set_difference(
            sorted_required_keys.begin(),
            sorted_required_keys.end(),
            sorted_object_keys.begin(),
            sorted_object_keys.end(),
            std::back_inserter(missing_keys));
        std::string msg;
        for (auto& key : missing_keys)
        {
          msg += "Missing required key '" + key + "' in object: " + out.c_str() + "\n";
        }

        std::cerr << msg << std::endl;
        return false;
      }

      std::vector<std::string> remaining;
      std::set_difference(
          difference.begin(), difference.end(), sorted_optional_keys.begin(), sorted_optional_keys.end(), std::back_inserter(remaining));

      // now, anything left must be standard comment starting with __
      for (auto& key : remaining)
      {
        if (!key.rfind("__", 0))
        {
          std::string msg = "Non-standard key '" + key + "' found in object" + out.c_str();
          std::cerr << msg << std::endl;
          return false;
        }
      }
      return true;
    }

    bool ParseRelativeTolerance(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (! ValidateSchema(object, { "value", "type" }, {})) {
        std::cerr << "Invalid schema for relative tolerance" << std::endl;
        return false;
      }
      mechanism->relative_tolerance = object["value"].as<double>();
      return true;
    }

    // void ParseMechanism(const YAML::Node& object)
    // {
    //   ValidateSchema(object, { "name", "reactions", "type" }, {});
    //   std::vector<YAML::Node> objects;
    //   for (const auto& element : object["reactions"])
    //   {
    //     objects.push_back(element);
    //   }
    //   ParseMechanismArray(objects);
    // }

    // std::vector<Species> ParseReactants(const YAML::Node& object)
    // {
    //   const std::string QTY = "qty";
    //   std::vector<Species> reactants;

    //   for (auto it = object.begin(); it != object.end(); ++it)
    //   {
    //     auto key = it->first.as<std::string>();
    //     auto value = it->second;

    //     std::size_t qty = 1;
    //     ValidateSchema(value, {}, { "qty" });
    //     if (value[QTY])
    //       qty = value[QTY].as<std::size_t>();
    //     for (std::size_t i = 0; i < qty; ++i)
    //       reactants.push_back(species_[key]);
    //   }
    //   return reactants;
    // }

    // std::vector<std::pair<Species, double>> ParseProducts(const YAML::Node& object)
    // {
    //   const std::string YIELD = "yield";

    //   constexpr double DEFAULT_YIELD = 1.0;
    //   std::vector<std::pair<Species, double>> products;
    //   for (auto it = object.begin(); it != object.end(); ++it)
    //   {
    //     auto key = it->first.as<std::string>();
    //     auto value = it->second;

    //     ValidateSchema(value, {}, { "yield" });
    //     auto species = species_[key];
    //     if (value[YIELD])
    //     {
    //       double yield = value[YIELD].as<double>();
    //       products.push_back(std::make_pair(species, yield));
    //     }
    //     else
    //     {
    //       products.push_back(std::make_pair(species, DEFAULT_YIELD));
    //     }
    //   }
    //   return products;
    // }

    // void ParsePhotolysis(const YAML::Node& object)
    // {
    //   const std::string REACTANTS = "reactants";
    //   const std::string PRODUCTS = "products";
    //   const std::string MUSICA_NAME = "MUSICA name";
    //   const std::string SCALING_FACTOR = "scaling factor";

    //   ValidateSchema(object, { "type", REACTANTS, PRODUCTS, MUSICA_NAME }, { SCALING_FACTOR });

    //   auto reactants = ParseReactants(object[REACTANTS]);
    //   auto products = ParseProducts(object[PRODUCTS]);
    //   double scaling_factor = object[SCALING_FACTOR] ? object[SCALING_FACTOR].as<double>() : 1.0;

    //   std::string name = "PHOTO." + object[MUSICA_NAME].as<std::string>();

    //   user_defined_rate_arr_.push_back(UserDefinedRateConstant({ .label_ = name, .scaling_factor_ = scaling_factor }));

    //   std::unique_ptr<UserDefinedRateConstant> rate_ptr =
    //       std::make_unique<UserDefinedRateConstant>(UserDefinedRateConstantParameters{ .label_ = name, .scaling_factor_ = scaling_factor });
    //   processes_.push_back(Process(reactants, products, std::move(rate_ptr), gas_phase_));
    // }

    // void ParseArrhenius(const YAML::Node& object)
    // {
    //   const std::string REACTANTS = "reactants";
    //   const std::string PRODUCTS = "products";

    //   ValidateSchema(object, { "type", REACTANTS, PRODUCTS }, { "A", "B", "C", "D", "E", "Ea", "MUSICA name" });

    //   auto reactants = ParseReactants(object[REACTANTS]);
    //   auto products = ParseProducts(object[PRODUCTS]);

    //   ArrheniusRateConstantParameters parameters;
    //   if (object["A"])
    //   {
    //     parameters.A_ = object["A"].as<double>();
    //   }
    //   parameters.A_ *= std::pow(MOLES_M3_TO_MOLECULES_CM3, reactants.size() - 1);
    //   if (object["B"])
    //   {
    //     parameters.B_ = object["B"].as<double>();
    //   }
    //   if (object["C"])
    //   {
    //     parameters.C_ = object["C"].as<double>();
    //   }
    //   if (object["D"])
    //   {
    //     parameters.D_ = object["D"].as<double>();
    //   }
    //   if (object["E"])
    //   {
    //     parameters.E_ = object["E"].as<double>();
    //   }
    //   if (object["Ea"])
    //   {
    //     if (parameters.C_ != 0)
    //     {
    //       throw std::system_error{ make_error_code(MicmConfigErrc::MutuallyExclusiveOption),
    //                                "Ea is specified when C is also specified for an Arrhenius reaction. Pick one." };
    //     }
    //     // Calculate 'C' using 'Ea'
    //     parameters.C_ = -1 * object["Ea"].as<double>() / constants::BOLTZMANN_CONSTANT;
    //   }
    //   arrhenius_rate_arr_.push_back(ArrheniusRateConstant(parameters));
    //   std::unique_ptr<ArrheniusRateConstant> rate_ptr = std::make_unique<ArrheniusRateConstant>(parameters);
    //   processes_.push_back(Process(reactants, products, std::move(rate_ptr), gas_phase_));
    // }

    // void ParseTroe(const YAML::Node& object)
    // {
    //   const std::string REACTANTS = "reactants";
    //   const std::string PRODUCTS = "products";

    //   ValidateSchema(object, { "type", REACTANTS, PRODUCTS }, { "k0_A", "k0_B", "k0_C", "kinf_A", "kinf_B", "kinf_C", "Fc", "N" });

    //   auto reactants = ParseReactants(object[REACTANTS]);
    //   auto products = ParseProducts(object[PRODUCTS]);

    //   TroeRateConstantParameters parameters;
    //   if (object["k0_A"])
    //   {
    //     parameters.k0_A_ = object["k0_A"].as<double>();
    //   }
    //   // Account for the conversion of reactant concentrations (including M) to molecules cm-3
    //   parameters.k0_A_ *= std::pow(MOLES_M3_TO_MOLECULES_CM3, reactants.size());
    //   if (object["k0_B"])
    //   {
    //     parameters.k0_B_ = object["k0_B"].as<double>();
    //   }
    //   if (object["k0_C"])
    //   {
    //     parameters.k0_C_ = object["k0_C"].as<double>();
    //   }
    //   if (object["kinf_A"])
    //   {
    //     parameters.kinf_A_ = object["kinf_A"].as<double>();
    //   }
    //   // Account for terms in denominator and exponent that include [M] but not other reactants
    //   parameters.kinf_A_ *= std::pow(MOLES_M3_TO_MOLECULES_CM3, reactants.size() - 1);
    //   if (object["kinf_B"])
    //   {
    //     parameters.kinf_B_ = object["kinf_B"].as<double>();
    //   }
    //   if (object["kinf_C"])
    //   {
    //     parameters.kinf_C_ = object["kinf_C"].as<double>();
    //   }
    //   if (object["Fc"])
    //   {
    //     parameters.Fc_ = object["Fc"].as<double>();
    //   }
    //   if (object["N"])
    //   {
    //     parameters.N_ = object["N"].as<double>();
    //   }
    //   troe_rate_arr_.push_back(TroeRateConstant(parameters));
    //   std::unique_ptr<TroeRateConstant> rate_ptr = std::make_unique<TroeRateConstant>(parameters);
    //   processes_.push_back(Process(reactants, products, std::move(rate_ptr), gas_phase_));
    // }

    // void ParseTernaryChemicalActivation(const YAML::Node& object)
    // {
    //   const std::string REACTANTS = "reactants";
    //   const std::string PRODUCTS = "products";

    //   ValidateSchema(object, { "type", REACTANTS, PRODUCTS }, { "k0_A", "k0_B", "k0_C", "kinf_A", "kinf_B", "kinf_C", "Fc", "N" });

    //   auto reactants = ParseReactants(object[REACTANTS]);
    //   auto products = ParseProducts(object[PRODUCTS]);

    //   TernaryChemicalActivationRateConstantParameters parameters;
    //   if (object["k0_A"])
    //   {
    //     parameters.k0_A_ = object["k0_A"].as<double>();
    //   }
    //   // Account for the conversion of reactant concentrations (including M) to molecules cm-3
    //   parameters.k0_A_ *= std::pow(MOLES_M3_TO_MOLECULES_CM3, reactants.size() - 1);
    //   if (object["k0_B"])
    //   {
    //     parameters.k0_B_ = object["k0_B"].as<double>();
    //   }
    //   if (object["k0_C"])
    //   {
    //     parameters.k0_C_ = object["k0_C"].as<double>();
    //   }
    //   if (object["kinf_A"])
    //   {
    //     parameters.kinf_A_ = object["kinf_A"].as<double>();
    //   }
    //   // Account for terms in denominator and exponent that include [M] but not other reactants
    //   parameters.kinf_A_ *= std::pow(MOLES_M3_TO_MOLECULES_CM3, reactants.size() - 2);
    //   if (object["kinf_B"])
    //   {
    //     parameters.kinf_B_ = object["kinf_B"].as<double>();
    //   }
    //   if (object["kinf_C"])
    //   {
    //     parameters.kinf_C_ = object["kinf_C"].as<double>();
    //   }
    //   if (object["Fc"])
    //   {
    //     parameters.Fc_ = object["Fc"].as<double>();
    //   }
    //   if (object["N"])
    //   {
    //     parameters.N_ = object["N"].as<double>();
    //   }

    //   ternary_rate_arr_.push_back(TernaryChemicalActivationRateConstant(parameters));
    //   std::unique_ptr<TernaryChemicalActivationRateConstant> rate_ptr = std::make_unique<TernaryChemicalActivationRateConstant>(parameters);
    //   processes_.push_back(Process(reactants, products, std::move(rate_ptr), gas_phase_));
    // }

    // void ParseBranched(const YAML::Node& object)
    // {
    //   const std::string REACTANTS = "reactants";
    //   const std::string ALKOXY_PRODUCTS = "alkoxy products";
    //   const std::string NITRATE_PRODUCTS = "nitrate products";
    //   const std::string X = "X";
    //   const std::string Y = "Y";
    //   const std::string A0 = "a0";
    //   const std::string N = "n";

    //   ValidateSchema(object, { "type", REACTANTS, ALKOXY_PRODUCTS, NITRATE_PRODUCTS, X, Y, A0, N }, {});

    //   auto reactants = ParseReactants(object[REACTANTS]);
    //   auto alkoxy_products = ParseProducts(object[ALKOXY_PRODUCTS]);
    //   auto nitrate_products = ParseProducts(object[NITRATE_PRODUCTS]);

    //   BranchedRateConstantParameters parameters;
    //   parameters.X_ = object[X].as<double>();
    //   // Account for the conversion of reactant concentrations to molecules cm-3
    //   parameters.X_ *= std::pow(MOLES_M3_TO_MOLECULES_CM3, reactants.size() - 1);
    //   parameters.Y_ = object[Y].as<double>();
    //   parameters.a0_ = object[A0].as<double>();
    //   parameters.n_ = object[N].as<int>();

    //   // Alkoxy branch
    //   parameters.branch_ = BranchedRateConstantParameters::Branch::Alkoxy;
    //   branched_rate_arr_.push_back(BranchedRateConstant(parameters));
    //   std::unique_ptr<BranchedRateConstant> rate_ptr = std::make_unique<BranchedRateConstant>(parameters);
    //   processes_.push_back(Process(reactants, alkoxy_products, std::move(rate_ptr), gas_phase_));

    //   // Nitrate branch
    //   parameters.branch_ = BranchedRateConstantParameters::Branch::Nitrate;
    //   branched_rate_arr_.push_back(BranchedRateConstant(parameters));
    //   rate_ptr = std::make_unique<BranchedRateConstant>(parameters);
    //   processes_.push_back(Process(reactants, nitrate_products, std::move(rate_ptr), gas_phase_));
    // }

    // void ParseTunneling(const YAML::Node& object)
    // {
    //   const std::string REACTANTS = "reactants";
    //   const std::string PRODUCTS = "products";

    //   ValidateSchema(object, { "type", REACTANTS, PRODUCTS }, { "A", "B", "C" });

    //   auto reactants = ParseReactants(object[REACTANTS]);
    //   auto products = ParseProducts(object[PRODUCTS]);

    //   TunnelingRateConstantParameters parameters;
    //   if (object["A"])
    //   {
    //     parameters.A_ = object["A"].as<double>();
    //   }
    //   // Account for the conversion of reactant concentrations to molecules cm-3
    //   parameters.A_ *= std::pow(MOLES_M3_TO_MOLECULES_CM3, reactants.size() - 1);
    //   if (object["B"])
    //   {
    //     parameters.B_ = object["B"].as<double>();
    //   }
    //   if (object["C"])
    //   {
    //     parameters.C_ = object["C"].as<double>();
    //   }

    //   tunneling_rate_arr_.push_back(TunnelingRateConstant(parameters));
    //   std::unique_ptr<TunnelingRateConstant> rate_ptr = std::make_unique<TunnelingRateConstant>(parameters);
    //   processes_.push_back(Process(reactants, products, std::move(rate_ptr), gas_phase_));
    // }

    // void ParseEmission(const YAML::Node& object)
    // {
    //   const std::string SPECIES = "species";
    //   const std::string MUSICA_NAME = "MUSICA name";
    //   const std::string PRODUCTS = "products";
    //   const std::string SCALING_FACTOR = "scaling factor";

    //   ValidateSchema(object, { "type", SPECIES, MUSICA_NAME }, { SCALING_FACTOR, PRODUCTS });

    //   std::string species = object["species"].as<std::string>();
    //   YAML::Node reactants_object{};
    //   YAML::Node products_object{};
    //   products_object[species]["yield"] = 1.0;
    //   auto reactants = ParseReactants(reactants_object);
    //   auto products = ParseProducts(products_object);
    //   double scaling_factor = object[SCALING_FACTOR] ? object[SCALING_FACTOR].as<double>() : 1.0;

    //   std::string name = "EMIS." + object[MUSICA_NAME].as<std::string>();
    //   user_defined_rate_arr_.push_back(UserDefinedRateConstant({ .label_ = name, .scaling_factor_ = scaling_factor }));
    //   std::unique_ptr<UserDefinedRateConstant> rate_ptr =
    //       std::make_unique<UserDefinedRateConstant>(UserDefinedRateConstantParameters{ .label_ = name, .scaling_factor_ = scaling_factor });
    //   processes_.push_back(Process(reactants, products, std::move(rate_ptr), gas_phase_));
    // }

    // void ParseFirstOrderLoss(const YAML::Node& object)
    // {
    //   const std::string SPECIES = "species";
    //   const std::string MUSICA_NAME = "MUSICA name";
    //   const std::string SCALING_FACTOR = "scaling factor";

    //   ValidateSchema(object, { "type", SPECIES, MUSICA_NAME }, { SCALING_FACTOR });

    //   std::string species = object["species"].as<std::string>();
    //   YAML::Node reactants_object{};
    //   YAML::Node products_object{};
    //   reactants_object[species] = {};
    //   auto reactants = ParseReactants(reactants_object);
    //   auto products = ParseProducts(products_object);
    //   double scaling_factor = object[SCALING_FACTOR] ? object[SCALING_FACTOR].as<double>() : 1.0;

    //   std::string name = "LOSS." + object[MUSICA_NAME].as<std::string>();
    //   user_defined_rate_arr_.push_back(UserDefinedRateConstant({ .label_ = name, .scaling_factor_ = scaling_factor }));
    //   std::unique_ptr<UserDefinedRateConstant> rate_ptr =
    //       std::make_unique<UserDefinedRateConstant>(UserDefinedRateConstantParameters{ .label_ = name, .scaling_factor_ = scaling_factor });
    //   processes_.push_back(Process(reactants, products, std::move(rate_ptr), gas_phase_));
    // }

    // void ParseUserDefined(const YAML::Node& object)
    // {
    //   const std::string REACTANTS = "reactants";
    //   const std::string PRODUCTS = "products";
    //   const std::string MUSICA_NAME = "MUSICA name";
    //   const std::string SCALING_FACTOR = "scaling factor";

    //   ValidateSchema(object, { "type", REACTANTS, PRODUCTS, MUSICA_NAME }, { SCALING_FACTOR });

    //   auto reactants = ParseReactants(object[REACTANTS]);
    //   auto products = ParseProducts(object[PRODUCTS]);
    //   double scaling_factor = object[SCALING_FACTOR] ? object[SCALING_FACTOR].as<double>() : 1.0;

    //   std::string name = "USER." + object[MUSICA_NAME].as<std::string>();
    //   user_defined_rate_arr_.push_back(UserDefinedRateConstant({ .label_ = name, .scaling_factor_ = scaling_factor }));
    //   std::unique_ptr<UserDefinedRateConstant> rate_ptr =
    //       std::make_unique<UserDefinedRateConstant>(UserDefinedRateConstantParameters{ .label_ = name, .scaling_factor_ = scaling_factor });
    //   processes_.push_back(Process(reactants, products, std::move(rate_ptr), gas_phase_));
    // }

    // void ParseSurface(const YAML::Node& object)
    // {
    //   const std::string REACTANTS = "gas-phase reactant";
    //   const std::string PRODUCTS = "gas-phase products";
    //   const std::string MUSICA_NAME = "MUSICA name";
    //   const std::string PROBABILITY = "reaction probability";

    //   ValidateSchema(object, { "type", REACTANTS, PRODUCTS, MUSICA_NAME }, { PROBABILITY });

    //   std::string species_name = object[REACTANTS].as<std::string>();
    //   YAML::Node reactants_object{};
    //   reactants_object[species_name] = {};

    //   auto reactants = ParseReactants(reactants_object);
    //   auto products = ParseProducts(object[PRODUCTS]);

    //   Species reactant_species = Species("");
    //   reactant_species = species_[species_name];
    //   SurfaceRateConstantParameters parameters{ .label_ = "SURF." + object[MUSICA_NAME].as<std::string>(), .species_ = reactant_species };

    //   if (object[PROBABILITY])
    //   {
    //     parameters.reaction_probability_ = object[PROBABILITY].as<double>();
    //   }

    //   surface_rate_arr_.push_back(SurfaceRateConstant(parameters));
    //   std::unique_ptr<SurfaceRateConstant> rate_ptr = std::make_unique<SurfaceRateConstant>(parameters);
    //   processes_.push_back(Process(reactants, products, std::move(rate_ptr), gas_phase_));
    // }

    // // Utility functions to check types and perform conversions
    // bool IsBool(const std::string& value)
    // {
    //   return (value == "true" || value == "false");
    // }

    // bool IsInt(const std::string& value)
    // {
    //   std::istringstream iss(value);
    //   int result;
    //   return (iss >> result >> std::ws).eof();  // Check if the entire string is an integer
    // }

    // bool IsFloat(const std::string& value)
    // {
    //   std::istringstream iss(value);
    //   float result;
    //   return (iss >> result >> std::ws).eof();  // Check if the entire string is a float
    // }

    /// @brief Search for nonstandard keys. Only nonstandard keys starting with __ are allowed. Others are considered typos
    /// @param object the object whose keys need to be validated
    /// @param required_keys The required keys
    /// @param optional_keys The optional keys
    bool ParseChemicalSpecies(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      // required keys
      const std::string NAME = "name";
      const std::string TYPE = "type";

      const std::string TRACER_TYPE = "tracer type";
      const std::string ABS_TOLERANCE = "absolute tolerance";
      const std::string DIFFUSION_COEFF = "diffusion coefficient [m2 s-1]";
      const std::string MOL_WEIGHT = "molecular weight [kg mol-1]";
      const std::string THIRD_BODY = "THIRD_BODY";

      if (!ValidateSchema(object, { NAME, TYPE }, { TRACER_TYPE, ABS_TOLERANCE, DIFFUSION_COEFF, MOL_WEIGHT }))
      {
        return false;
      }

      std::string name = object[NAME].as<std::string>();
      types::Species species;
      species.name = name;

      // Load remaining keys as properties
      for (auto it = object.begin(); it != object.end(); ++it)
      {
        auto key = it->first.as<std::string>();
        auto value = it->second;

        if (key.empty())
        {
          std::cerr << "Empty key found in object: " << object << std::endl;
          return false;
        }

        if (key != NAME && key != TYPE)
        {
          std::string stringValue = value.as<std::string>();
          species.unknown_properties[key] = stringValue;

          if (key == TRACER_TYPE && stringValue == THIRD_BODY)
          {
            species.third_body = true;
          }
        }
      }
      mechanism->species.push_back(species);

      return true;
    }

    bool ParseSpeciesArray(std::unique_ptr<types::Mechanism>& mechanism, const std::vector<YAML::Node>& objects)
    {
      const std::string TYPE = "type";
      for (const auto& object : objects)
      {
        std::string type = object[TYPE].as<std::string>();

        if (type == "CHEM_SPEC")
        {
          if (!ParseChemicalSpecies(mechanism, object))
          {
            return false;
          }
        }
        else if (type == "RELATIVE_TOLERANCE")
        {
          if (! ParseRelativeTolerance(mechanism, object)) {
            return false;
          }
        }
      }
      return true;
    }

    std::optional<std::unique_ptr<GlobalMechanism>> Parser::TryParse(const std::filesystem::path& config_path)
    {
      return std::nullopt;

      std::unique_ptr<types::Mechanism> mechanism = std::make_unique<types::Mechanism>();
      // Look for CAMP config path
      if (!std::filesystem::exists(config_path))
      {
        std::cerr << "File does not exist: " << config_path << std::endl;
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
        std::cerr << "CAMP files not found in: " << config_file << std::endl;
        return std::nullopt;
      }

      // Build a list of individual CAMP config files
      std::vector<std::filesystem::path> camp_files;
      for (const auto& element : camp_data[CAMP_FILES])
      {
        std::filesystem::path camp_file = config_dir / element.as<std::string>();
        if (!std::filesystem::exists(camp_file))
        {
          std::cerr << "CAMP file not found: " << camp_file << std::endl;
          return std::nullopt;
        }
        camp_files.push_back(camp_file);
      }

      // No config files found
      if (camp_files.size() < 1)
      {
        std::cerr << "No CAMP files found in: " << config_file << std::endl;
        return std::nullopt;
      }

      std::vector<YAML::Node> species_objects;
      std::vector<YAML::Node> mechanism_objects;

      // Iterate CAMP file list and form CAMP data object arrays
      for (const auto& camp_file : camp_files)
      {
        YAML::Node config_subset = YAML::LoadFile(camp_file.string());

        if (!config_subset[CAMP_DATA])
        {
          YAML::Emitter out;
          out << config_subset;
          std::cerr << "CAMP data not found in: " << camp_file << std::endl;
          return std::nullopt;
        }
        // Iterate YAML objects from CAMP data entry
        for (const auto& object : config_subset[CAMP_DATA])
        {
          if (object)
          {
            // Require object to have a type entry
            if (!object[TYPE])
            {
              YAML::Emitter out;
              out << object;
              std::string msg = "object: " + std::string(out.c_str()) + "; type: " + TYPE;

              std::cerr << "Object type not found in: " << camp_file << std::endl;
              return std::nullopt;
            }

            // Sort into object arrays by type
            std::string type = object[TYPE].as<std::string>();
            // CHEM_SPEC and RELATIVE_TOLERANCE parsed first by ParseSpeciesArray
            if ((type == "CHEM_SPEC") || (type == "RELATIVE_TOLERANCE"))
            {
              species_objects.push_back(object);
            }
            // All other objects will be parsed by ParseMechanismArray
            else
            {
              mechanism_objects.push_back(object);
            }
          }
        }
      }

      // Parse species object array
      if (!ParseSpeciesArray(mechanism, species_objects))
      {
        std::cerr << "Failed to parse species array" << std::endl;
        return std::nullopt;
      }

      //   // Assign the parsed 'Species' to 'Phase'
      //   std::vector<Species> species_arr;
      //   for (const auto& [name, species] : species_)
      //   {
      //     species_arr.push_back(species);
      //   }
      //   gas_phase_ = Phase(species_arr);

      //   // Parse mechanism object array
      //   ParseMechanismArray(mechanism_objects);
    }

  }  // namespace v0
}  // namespace mechanism_configuration

// Solver parameters
// class ConfigReaderPolicy
// {
//   using YAML::Node = YAML::Node;

//  public:
//   std::map<std::string, Species> species_;

//   std::vector<UserDefinedRateConstant> user_defined_rate_arr_;
//   std::vector<ArrheniusRateConstant> arrhenius_rate_arr_;
//   std::vector<TroeRateConstant> troe_rate_arr_;
//   std::vector<TernaryChemicalActivationRateConstant> ternary_rate_arr_;
//   std::vector<BranchedRateConstant> branched_rate_arr_;
//   std::vector<TunnelingRateConstant> tunneling_rate_arr_;
//   std::vector<SurfaceRateConstant> surface_rate_arr_;

//   // Specific for solver parameters
//   Phase gas_phase_;
//   std::unordered_map<std::string, Phase> phases_;
//   std::vector<Process> processes_;
//   RosenbrockSolverParameters parameters_;
//   double relative_tolerance_;

//   // Common YAML

//   // Error string
//   std::stringstream last_json_object_;

//   // Constructor

//   ConfigReaderPolicy(const RosenbrockSolverParameters& parameters)
//       : parameters_(parameters)
//   {
//   }

//   // Functions

//   /// @brief Parse configures
//   /// @param config_path Path to a the CAMP configuration directory or file

//  private:

//   void ParseMechanismArray(const std::vector<YAML::Node>& objects)
//   {
//     for (const auto& object : objects)
//     {
//       std::string type = object[TYPE].as<std::string>();

//       if (type == "MECHANISM")
//       {
//         ParseMechanism(object);
//       }
//       else if (type == "PHOTOLYSIS")
//       {
//         ParsePhotolysis(object);
//       }
//       else if (type == "EMISSION")
//       {
//         ParseEmission(object);
//       }
//       else if (type == "FIRST_ORDER_LOSS")
//       {
//         ParseFirstOrderLoss(object);
//       }
//       else if (type == "ARRHENIUS")
//       {
//         ParseArrhenius(object);
//       }
//       else if (type == "TROE")
//       {
//         ParseTroe(object);
//       }
//       else if (type == "TERNARY_CHEMICAL_ACTIVATION")
//       {
//         ParseTernaryChemicalActivation(object);
//       }
//       else if (type == "BRANCHED" || type == "WENNBERG_NO_RO2")
//       {
//         ParseBranched(object);
//       }
//       else if (type == "TUNNELING" || type == "WENNBERG_TUNNELING")
//       {
//         ParseTunneling(object);
//       }
//       else if (type == "SURFACE")
//       {
//         ParseSurface(object);
//       }
//       else if (type == "USER_DEFINED")
//       {
//         ParseUserDefined(object);
//       }
//       else
//       {
//         throw std::system_error{ make_error_code(MicmConfigErrc::UnknownKey), type };
//       }
//     }
//   }

// };

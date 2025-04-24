// Copyright (C) 2025 University Corporation for Atmospheric Research
// SPDX-License-Identifier: Apache-2.0
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/types.hpp>
#include <mechanism_configuration/v1/validation.hpp>
#include <variant>

namespace py = pybind11;
namespace constants = mechanism_configuration::constants;
namespace validation = mechanism_configuration::v1::validation;
using namespace mechanism_configuration::v1::types;

enum class ReactionType
{
  Arrhenius,
  Branched,
  CondensedPhaseArrhenius,
  CondensedPhasePhotolysis,
  Emission,
  FirstOrderLoss,
  SimpolPhaseTransfer,
  AqueousEquilibrium,
  WetDeposition,
  HenrysLaw,
  Photolysis,
  Surface,
  Troe,
  Tunneling
};

struct ReactionsIterator
{
  using VariantType = std::variant<
      Arrhenius,
      Branched,
      CondensedPhaseArrhenius,
      CondensedPhasePhotolysis,
      Emission,
      FirstOrderLoss,
      SimpolPhaseTransfer,
      AqueousEquilibrium,
      WetDeposition,
      HenrysLaw,
      Photolysis,
      Surface,
      Troe,
      Tunneling>;

  std::vector<std::vector<VariantType>> reaction_lists;
  size_t outer_index = 0;
  size_t inner_index = 0;

  ReactionsIterator(Reactions &reactions)
      : reaction_lists{ std::vector<VariantType>(reactions.arrhenius.begin(), reactions.arrhenius.end()),
                        std::vector<VariantType>(reactions.branched.begin(), reactions.branched.end()),
                        std::vector<VariantType>(reactions.condensed_phase_arrhenius.begin(), reactions.condensed_phase_arrhenius.end()),
                        std::vector<VariantType>(reactions.condensed_phase_photolysis.begin(), reactions.condensed_phase_photolysis.end()),
                        std::vector<VariantType>(reactions.emission.begin(), reactions.emission.end()),
                        std::vector<VariantType>(reactions.first_order_loss.begin(), reactions.first_order_loss.end()),
                        std::vector<VariantType>(reactions.simpol_phase_transfer.begin(), reactions.simpol_phase_transfer.end()),
                        std::vector<VariantType>(reactions.aqueous_equilibrium.begin(), reactions.aqueous_equilibrium.end()),
                        std::vector<VariantType>(reactions.wet_deposition.begin(), reactions.wet_deposition.end()),
                        std::vector<VariantType>(reactions.henrys_law.begin(), reactions.henrys_law.end()),
                        std::vector<VariantType>(reactions.photolysis.begin(), reactions.photolysis.end()),
                        std::vector<VariantType>(reactions.surface.begin(), reactions.surface.end()),
                        std::vector<VariantType>(reactions.troe.begin(), reactions.troe.end()),
                        std::vector<VariantType>(reactions.tunneling.begin(), reactions.tunneling.end()) }
  {
  }

  py::object next()
  {
    while (outer_index < reaction_lists.size())
    {
      const auto &vec = reaction_lists[outer_index];
      if (inner_index < vec.size())
      {
        return std::visit([](auto &&arg) { return py::cast(arg); }, vec[inner_index++]);
      }
      ++outer_index;
      inner_index = 0;
    }
    throw py::stop_iteration();
  }
};

std::vector<ReactionComponent> get_reaction_components(const py::list& components)
{
  std::vector<ReactionComponent> reaction_components;
  for (const auto &item : components) {
    if (py::isinstance<Species>(item)) {
        reaction_components.push_back(ReactionComponent(item.cast<Species>().name));
    } else if (py::isinstance<py::tuple>(item) && py::len(item.cast<py::tuple>()) == 2) {
        auto item_tuple = item.cast<py::tuple>();
        if (py::isinstance<py::float_>(item_tuple[0]) && py::isinstance<Species>(item_tuple[1])) {
            double coefficient = item_tuple[0].cast<double>();
            Species species = item_tuple[1].cast<Species>();
            reaction_components.push_back(ReactionComponent(species.name, coefficient));
        } else if (py::isinstance<py::int_>(item_tuple[0]) && py::isinstance<Species>(item_tuple[1])) {
            double coefficient = item_tuple[0].cast<int>();
            Species species = item_tuple[1].cast<Species>();
            reaction_components.push_back(ReactionComponent(species.name, coefficient));
        } else {
            throw py::value_error("Invalid tuple format. Expected (float, Species).");
        }
    } else {
        throw py::value_error("Invalid type for reactant. Expected a Species or a tuple of (float, Species).");
    }
  }
  std::unordered_set<std::string> component_names;
  for (const auto &component : reaction_components) {
    if (!component_names.insert(component.species_name).second) {
        throw py::value_error("Duplicate reaction component name found: " + component.species_name);
    }
  }
  return reaction_components;
}

Reactions create_reactions(const py::list& reactions)
{
  Reactions reaction_obj;
  for (const auto &item : reactions) {
    if (py::isinstance<Arrhenius>(item)) {
      reaction_obj.arrhenius.push_back(item.cast<Arrhenius>());
    } else if (py::isinstance<Branched>(item)) {
      reaction_obj.branched.push_back(item.cast<Branched>());
    } else if (py::isinstance<CondensedPhaseArrhenius>(item)) {
      reaction_obj.condensed_phase_arrhenius.push_back(item.cast<CondensedPhaseArrhenius>());
    } else if (py::isinstance<CondensedPhasePhotolysis>(item)) {
      reaction_obj.condensed_phase_photolysis.push_back(item.cast<CondensedPhasePhotolysis>());
    } else if (py::isinstance<Emission>(item)) {
      reaction_obj.emission.push_back(item.cast<Emission>());
    } else if (py::isinstance<FirstOrderLoss>(item)) {
      reaction_obj.first_order_loss.push_back(item.cast<FirstOrderLoss>());
    } else if (py::isinstance<SimpolPhaseTransfer>(item)) {
      reaction_obj.simpol_phase_transfer.push_back(item.cast<SimpolPhaseTransfer>());
    } else if (py::isinstance<AqueousEquilibrium>(item)) {
      reaction_obj.aqueous_equilibrium.push_back(item.cast<AqueousEquilibrium>());
    } else if (py::isinstance<WetDeposition>(item)) {
      reaction_obj.wet_deposition.push_back(item.cast<WetDeposition>());
    } else if (py::isinstance<HenrysLaw>(item)) {
      reaction_obj.henrys_law.push_back(item.cast<HenrysLaw>());
    } else if (py::isinstance<Photolysis>(item)) {
      reaction_obj.photolysis.push_back(item.cast<Photolysis>());
    } else if (py::isinstance<Surface>(item)) {
      reaction_obj.surface.push_back(item.cast<Surface>());
    } else if (py::isinstance<Troe>(item)) {
      reaction_obj.troe.push_back(item.cast<Troe>());
    } else if (py::isinstance<Tunneling>(item)) {
      reaction_obj.tunneling.push_back(item.cast<Tunneling>());
    } else {
        throw py::value_error("Invalid reaction type.");
    }
  }
  return reaction_obj;
}

PYBIND11_MODULE(_mechanism_configuration, m)
{
  py::enum_<ReactionType>(m, "ReactionType")
      .value("Arrhenius", ReactionType::Arrhenius)
      .value("Branched", ReactionType::Branched)
      .value("CondensedPhaseArrhenius", ReactionType::CondensedPhaseArrhenius)
      .value("CondensedPhasePhotolysis", ReactionType::CondensedPhasePhotolysis)
      .value("Emission", ReactionType::Emission)
      .value("FirstOrderLoss", ReactionType::FirstOrderLoss)
      .value("SimpolPhaseTransfer", ReactionType::SimpolPhaseTransfer)
      .value("AqueousEquilibrium", ReactionType::AqueousEquilibrium)
      .value("WetDeposition", ReactionType::WetDeposition)
      .value("HenrysLaw", ReactionType::HenrysLaw)
      .value("Photolysis", ReactionType::Photolysis)
      .value("Surface", ReactionType::Surface)
      .value("Troe", ReactionType::Troe)
      .value("Tunneling", ReactionType::Tunneling);

  py::module_ core = m.def_submodule("_core");

  py::class_<Species>(core, "_Species")
      .def(py::init<>())
      .def_static(
          "from_dict",
          [](const std::map<std::string, py::object> &properties)
          {
            Species species;

          // Iterate through the dictionary and set known properties
          for (const auto &[key, value] : properties) {
              try {
                  if (key == validation::name) {
                      species.name = value.cast<std::string>();
                  } else if (key == validation::absolute_tolerance) {
                      species.absolute_tolerance = value.cast<double>();
                  } else if (key == validation::diffusion_coefficient) {
                      species.diffusion_coefficient = value.cast<double>();
                  } else if (key == validation::molecular_weight) {
                      species.molecular_weight = value.cast<double>();
                  } else if (key == validation::henrys_law_constant_298) {
                      species.henrys_law_constant_298 = value.cast<double>();
                  } else if (key == validation::henrys_law_constant_exponential_factor) {
                      species.henrys_law_constant_exponential_factor = value.cast<double>();
                  } else if (key == validation::n_star) {
                      species.n_star = value.cast<double>();
                  } else if (key == validation::density) {
                      species.density = value.cast<double>();
                  } else if (key == validation::tracer_type) {
                      species.tracer_type = value.cast<std::string>();
                  } else {
                      // Add unmatched properties to unknown_properties
                      if (key.rfind("__", 0) != 0) {
                          throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                      }
                      species.unknown_properties[key] = py::str(value);
                  }
              } catch (const py::cast_error &e) {
                  throw py::value_error("Invalid type for property '" + key + "'. Expected a double or string.");
              }
            }
            return species;
          })
      .def_readwrite("name", &Species::name)
      .def_readwrite("absolute_tolerance", &Species::absolute_tolerance)
      .def_readwrite("diffusion_coefficient_m2_s", &Species::diffusion_coefficient)
      .def_readwrite("molecular_weight_kg_mol", &Species::molecular_weight)
      .def_readwrite("HLC_298K_mol_m3_Pa", &Species::henrys_law_constant_298)
      .def_readwrite("HLC_exponential_factor_K", &Species::henrys_law_constant_exponential_factor)
      .def_readwrite("N_star", &Species::n_star)
      .def_readwrite("density_kg_m3", &Species::density)
      .def_readwrite("tracer_type", &Species::tracer_type)
      .def_readwrite("unknown_properties", &Species::unknown_properties)
      .def("__str__", [](const Species &s) { return s.name; })
      .def("__repr__", [](const Species &s) { return "<Species: " + s.name + ">"; });

  py::class_<Phase>(core, "_Phase")
      .def(py::init<>())
      .def_static(
          "from_dict",
          [](const std::map<std::string, py::object> &properties)
          {
            Phase phase;

            // Iterate through the dictionary and set known properties
            for (const auto &[key, value] : properties)
            {
              try
              {
                if (key == validation::name)
                {
                  phase.name = value.cast<std::string>();
                }
                else if (key == validation::species)
                {
                  auto species_list = value.cast<py::list>();
                  for (const auto &spec : species_list)
                  {
                    auto species = spec.cast<Species>();
                    phase.species.push_back(species.name);
                  }
                }
                else
                {
                  // Add unmatched properties to unknown_properties
                  if (key.rfind("__", 0) != 0)
                  {
                    throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                  }
                  phase.unknown_properties[key] = py::str(value);
                }
              }
              catch (const py::cast_error &e)
              {
                throw py::value_error("Invalid type for property '" + key + "'. Expected a string.");
              }
            }
            return phase;
          })
      .def_readwrite("name", &Phase::name)
      .def_readwrite("species", &Phase::species)
      .def_readwrite("unknown_properties", &Phase::unknown_properties)
      .def("__str__", [](const Phase &p) { return p.name; })
      .def("__repr__", [](const Phase &p) { return "<Phase: " + p.name + ">"; });

  py::class_<ReactionComponent>(core, "_ReactionComponent")
      .def(py::init<>())
      .def(py::init([](const std::string &species_name) {
          ReactionComponent rc;
          rc.species_name = species_name;
          return rc;
      }))
      .def(py::init([](const std::string &species_name, double coefficient) {
          ReactionComponent rc;
          rc.species_name = species_name;
          rc.coefficient = coefficient;
          return rc;
      }))
      .def_readwrite("species_name", &ReactionComponent::species_name)
      .def_readwrite("coefficient", &ReactionComponent::coefficient)
      .def_readwrite("unknown_properties", &ReactionComponent::unknown_properties)
      .def("__str__", [](const ReactionComponent &rc) { return rc.species_name; })
      .def("__repr__", [](const ReactionComponent &rc) { return "<ReactionComponent: " + rc.species_name + ">"; });

  py::class_<Arrhenius>(core, "_Arrhenius")
      .def(py::init<>())
      .def("from_dict",
        [](const std::map<std::string, py::object> &properties) {
            Arrhenius arrhenius;
    
            // Iterate through the dictionary and set known properties
            for (const auto &[key, value] : properties) {
                try {
                    if (key == validation::name) {
                        arrhenius.name = value.cast<std::string>();
                    } else if (key == validation::A) {
                        arrhenius.A = value.cast<double>();
                    } else if (key == validation::B) {
                        arrhenius.B = value.cast<double>();
                    } else if (key == validation::C) {
                        if (arrhenius.C != 0) {
                            throw py::value_error("Mutually exclusive option: Ea and C");
                        }
                        arrhenius.C = value.cast<double>();
                    } else if (key == validation::Ea) {
                        if (arrhenius.C != 0) {
                            throw py::value_error("Mutually exclusive option: Ea and C");
                        }
                        arrhenius.C = -1 * value.cast<double>() / constants::boltzmann;
                    } else if (key == validation::D) {
                        arrhenius.D = value.cast<double>();
                    } else if (key == validation::E) {
                        arrhenius.E = value.cast<double>();
                    } else if (key == validation::reactants) {
                        if (!py::isinstance<py::list>(value)) {
                            throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                        }
                        arrhenius.reactants = get_reaction_components(value.cast<py::list>());
                    } else if (key == validation::products) {
                        if (!py::isinstance<py::list>(value)) {
                            throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                        }
                        arrhenius.products = get_reaction_components(value.cast<py::list>());
                    } else if (key == validation::gas_phase) {
                        arrhenius.gas_phase = value.cast<Phase>().name;
                    } else {
                        // Add unmatched properties to unknown_properties
                        if (key.rfind("__", 0) != 0) {
                            throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                        }
                        arrhenius.unknown_properties[key] = py::str(value);
                    }
                } catch (const py::cast_error &e) {
                    throw py::value_error("Invalid type for property '" + key + "'.");
                }
            }
            return arrhenius;
        })
      .def_readwrite("A", &Arrhenius::A)
      .def_readwrite("B", &Arrhenius::B)
      .def_readwrite("C", &Arrhenius::C)
      .def_readwrite("D", &Arrhenius::D)
      .def_readwrite("E", &Arrhenius::E)
      .def_readwrite("reactants", &Arrhenius::reactants)
      .def_readwrite("products", &Arrhenius::products)
      .def_readwrite("name", &Arrhenius::name)
      .def_readwrite("gas_phase", &Arrhenius::gas_phase)
      .def_readwrite("unknown_properties", &Arrhenius::unknown_properties)
      .def("__str__", [](const Arrhenius &a) { return a.name; })
      .def("__repr__", [](const Arrhenius &a) { return "<Arrhenius: " + a.name + ">"; })
      .def_property_readonly("type", [](const Arrhenius &) { return ReactionType::Arrhenius; });

  py::class_<CondensedPhaseArrhenius>(m, "CondensedPhaseArrhenius")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            CondensedPhaseArrhenius cpa;
            cpa.name = name;
            return cpa;
        }))
      .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
          CondensedPhaseArrhenius cpa;
          cpa.name = name;

          // Iterate through the dictionary and set known properties
          for (const auto &[key, value] : properties) {
              try {
                  if (key == validation::A) {
                      cpa.A = value.cast<double>();
                  } else if (key == validation::B) {
                      cpa.B = value.cast<double>();
                  } else if (key == validation::C) {
                      if (cpa.C != 0) {
                          throw py::value_error("Mutually exclusive option: Ea and C");
                      }
                      cpa.C = value.cast<double>();
                  } else if (key == validation::Ea) {
                      if (cpa.C != 0) {
                          throw py::value_error("Mutually exclusive option: Ea and C");
                      }
                      cpa.C = -1 * value.cast<double>() / constants::boltzmann;
                  } else if (key == validation::D) {
                      cpa.D = value.cast<double>();
                  } else if (key == validation::E) {
                      cpa.E = value.cast<double>();
                  } else if (key == validation::reactants) {
                      if (!py::isinstance<py::list>(value)) {
                          throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                      }
                      cpa.reactants = get_reaction_components(value.cast<py::list>());
                  } else if (key == validation::products) {
                      if (!py::isinstance<py::list>(value)) {
                          throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                      }
                      cpa.products = get_reaction_components(value.cast<py::list>());
                  } else if (key == validation::aerosol_phase_water) {
                      cpa.aerosol_phase_water = value.cast<Species>().name;
                  } else if (key == validation::aerosol_phase) {
                      cpa.aerosol_phase = value.cast<Phase>().name;
                  } else {
                      // Add unmatched properties to unknown_properties
                      if
                          (key.rfind("__", 0) != 0) {
                          throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                      }
                      cpa.unknown_properties[key] = py::str(value);
                  }
              } catch (const py::cast_error &e) {
                  throw py::value_error("Invalid type for property '" + key + "'.");
              }
          }
          return cpa;
      }))
      .def_readwrite("A", &CondensedPhaseArrhenius::A)
      .def_readwrite("B", &CondensedPhaseArrhenius::B)
      .def_readwrite("C", &CondensedPhaseArrhenius::C)
      .def_readwrite("D", &CondensedPhaseArrhenius::D)
      .def_readwrite("E", &CondensedPhaseArrhenius::E)
      .def_readwrite("reactants", &CondensedPhaseArrhenius::reactants)
      .def_readwrite("products", &CondensedPhaseArrhenius::products)
      .def_readwrite("name", &CondensedPhaseArrhenius::name)
      .def_readwrite("aerosol_phase", &CondensedPhaseArrhenius::aerosol_phase)
      .def_readwrite("aerosol_phase_water", &CondensedPhaseArrhenius::aerosol_phase_water)
      .def_readwrite("unknown_properties", &CondensedPhaseArrhenius::unknown_properties)
      .def("__str__", [](const CondensedPhaseArrhenius &cpa) { return cpa.name; })
      .def("__repr__", [](const CondensedPhaseArrhenius &cpa) { return "<CondensedPhaseArrhenius: " + cpa.name + ">"; })
      .def_property_readonly("type", [](const CondensedPhaseArrhenius &) { return ReactionType::CondensedPhaseArrhenius; });

  py::class_<Troe>(m, "Troe")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            Troe troe;
            troe.name = name;
            return troe;
        }))
        .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
            Troe troe;
            troe.name = name;
    
            // Iterate through the dictionary and set known properties
            for (const auto &[key, value] : properties) {
                try {
                    if (key == validation::k0_A) {
                        troe.k0_A = value.cast<double>();
                    } else if (key == validation::k0_B) {
                        troe.k0_B = value.cast<double>();
                    } else if (key == validation::k0_C) {
                        troe.k0_C = value.cast<double>();
                    } else if (key == validation::kinf_A) {
                        troe.kinf_A = value.cast<double>();
                    } else if (key == validation::kinf_B) {
                        troe.kinf_B = value.cast<double>();
                    } else if (key == validation::kinf_C) {
                        troe.kinf_C = value.cast<double>();
                    } else if (key == validation::Fc) {
                        troe.Fc = value.cast<double>();
                    } else if (key == validation::N) {
                        troe.N = value.cast<double>();
                    } else if (key == validation::reactants) {
                        if (!py::isinstance<py::list>(value)) {
                            throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                        }
                        troe.reactants = get_reaction_components(value.cast<py::list>());
                    } else if (key == validation::products) {
                        if (!py::isinstance<py::list>(value)) {
                            throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                        }
                        troe.products = get_reaction_components(value.cast<py::list>());
                    } else if (key == validation::gas_phase) {
                        troe.gas_phase = value.cast<Phase>().name;
                    } else {
                        // Add unmatched properties to unknown_properties
                        if (key.rfind("__", 0) != 0) {
                            throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                        }
                        troe.unknown_properties[key] = py::str(value);
                    }
                } catch (const py::cast_error &e) {
                    throw py::value_error("Invalid type for property '" + key + "'.");
                }
            }
            return troe;
        }))
      .def_readwrite("k0_A", &Troe::k0_A)
      .def_readwrite("k0_B", &Troe::k0_B)
      .def_readwrite("k0_C", &Troe::k0_C)
      .def_readwrite("kinf_A", &Troe::kinf_A)
      .def_readwrite("kinf_B", &Troe::kinf_B)
      .def_readwrite("kinf_C", &Troe::kinf_C)
      .def_readwrite("Fc", &Troe::Fc)
      .def_readwrite("N", &Troe::N)
      .def_readwrite("reactants", &Troe::reactants)
      .def_readwrite("products", &Troe::products)
      .def_readwrite("name", &Troe::name)
      .def_readwrite("gas_phase", &Troe::gas_phase)
      .def_readwrite("unknown_properties", &Troe::unknown_properties)
      .def("__str__", [](const Troe &t) { return t.name; })
      .def("__repr__", [](const Troe &t) { return "<Troe: " + t.name + ">"; })
      .def_property_readonly("type", [](const Troe &) { return ReactionType::Troe; });

  py::class_<Branched>(m, "Branched")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            Branched branched;
            branched.name = name;
            return branched;
        }))
        .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
            Branched branched;
            branched.name = name;

            // Iterate through the dictionary and set known properties
            for (const auto &[key, value] : properties) {
                try {
                    if (key == validation::X) {
                        branched.X = value.cast<double>();
                    } else if (key == validation::Y) {
                        branched.Y = value.cast<double>();
                    } else if (key == validation::a0) {
                        branched.a0 = value.cast<double>();
                    } else if (key == validation::n) {
                        branched.n = value.cast<int>();
                    } else if (key == validation::reactants) {
                        if (!py::isinstance<py::list>(value)) {
                            throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                        }
                        branched.reactants = get_reaction_components(value.cast<py::list>());
                    } else if (key == validation::nitrate_products) {
                        if (!py::isinstance<py::list>(value)) {
                            throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                        }
                        branched.nitrate_products = get_reaction_components(value.cast<py::list>());
                    } else if (key == validation::alkoxy_products) {
                        if (!py::isinstance<py::list>(value)) {
                            throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                        }
                        branched.alkoxy_products = get_reaction_components(value.cast<py::list>());
                    } else if (key == validation::gas_phase) {
                        branched.gas_phase = value.cast<Phase>().name;
                    } else {
                        // Add unmatched properties to unknown_properties
                        if (key.rfind("__", 0) != 0) {
                            throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                        }
                        branched.unknown_properties[key] = py::str(value);
                    }
                } catch (const py::cast_error &e) {
                    throw py::value_error("Invalid type for property '" + key + "'.");
                }
            }
            return branched;
        }))
      .def_readwrite("X", &Branched::X)
      .def_readwrite("Y", &Branched::Y)
      .def_readwrite("a0", &Branched::a0)
      .def_readwrite("n", &Branched::n)
      .def_readwrite("reactants", &Branched::reactants)
      .def_readwrite("nitrate_products", &Branched::nitrate_products)
      .def_readwrite("alkoxy_products", &Branched::alkoxy_products)
      .def_readwrite("name", &Branched::name)
      .def_readwrite("gas_phase", &Branched::gas_phase)
      .def_readwrite("unknown_properties", &Branched::unknown_properties)
      .def("__str__", [](const Branched &b) { return b.name; })
      .def("__repr__", [](const Branched &b) { return "<Branched: " + b.name + ">"; })
      .def_property_readonly("type", [](const Branched &) { return ReactionType::Branched; });

  py::class_<Tunneling>(m, "Tunneling")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            Tunneling tunneling;
            tunneling.name = name;
            return tunneling;
        }))
            .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
                Tunneling tunneling;
                tunneling.name = name;
    
                // Iterate through the dictionary and set known properties
                for (const auto &[key, value] : properties) {
                    try {
                        if (key == validation::A) {
                            tunneling.A = value.cast<double>();
                        } else if (key == validation::B) {
                            tunneling.B = value.cast<double>();
                        } else if (key == validation::C) {
                            tunneling.C = value.cast<double>();
                        } else if (key == validation::reactants) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            tunneling.reactants = get_reaction_components(value.cast<py::list>());
                        } else if (key == validation::products) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            tunneling.products = get_reaction_components(value.cast<py::list>());
                        } else if (key == validation::gas_phase) {
                            tunneling.gas_phase = value.cast<Phase>().name;
                        } else {
                            // Add unmatched properties to unknown_properties
                            if (key.rfind("__", 0) != 0) {
                                throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                            }
                            tunneling.unknown_properties[key] = py::str(value);
                        }
                    } catch (const py::cast_error &e) {
                        throw py::value_error("Invalid type for property '" + key + "'.");
                    }
                }
                return tunneling;
            }))
      .def_readwrite("A", &Tunneling::A)
      .def_readwrite("B", &Tunneling::B)
      .def_readwrite("C", &Tunneling::C)
      .def_readwrite("reactants", &Tunneling::reactants)
      .def_readwrite("products", &Tunneling::products)
      .def_readwrite("name", &Tunneling::name)
      .def_readwrite("gas_phase", &Tunneling::gas_phase)
      .def_readwrite("unknown_properties", &Tunneling::unknown_properties)
      .def("__str__", [](const Tunneling &t) { return t.name; })
      .def("__repr__", [](const Tunneling &t) { return "<Tunneling: " + t.name + ">"; })
      .def_property_readonly("type", [](const Tunneling &) { return ReactionType::Tunneling; });

  py::class_<Surface>(m, "Surface")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            Surface surface;
            surface.name = name;
            return surface;
        }))
            .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
                Surface surface;
                surface.name = name;
    
                // Iterate through the dictionary and set known properties
                for (const auto &[key, value] : properties) {
                    try {
                        if (key == validation::reaction_probability) {
                            surface.reaction_probability = value.cast<double>();
                        } else if (key == validation::gas_phase_species) {
                            surface.gas_phase_species = ReactionComponent(value.cast<Species>().name);
                        } else if (key == validation::gas_phase_products) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            surface.gas_phase_products = get_reaction_components(value.cast<py::list>());
                        } else if (key == validation::gas_phase) {
                            surface.gas_phase = value.cast<Phase>().name;
                        } else if (key == validation::aerosol_phase) {
                            surface.aerosol_phase = value.cast<Phase>().name;
                        } else {
                            // Add unmatched properties to unknown_properties
                            if (key.rfind("__", 0) != 0) {
                                throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                            }
                            surface.unknown_properties[key] = py::str(value);
                        }
                    } catch (const py::cast_error &e) {
                        throw py::value_error("Invalid type for property '" + key + "'.");
                    }
                }
                return surface;
            }))
      .def_readwrite("reaction_probability", &Surface::reaction_probability)
      .def_readwrite("gas_phase_species", &Surface::gas_phase_species)
      .def_readwrite("gas_phase_products", &Surface::gas_phase_products)
      .def_readwrite("name", &Surface::name)
      .def_readwrite("gas_phase", &Surface::gas_phase)
      .def_readwrite("aerosol_phase", &Surface::aerosol_phase)
      .def_readwrite("unknown_properties", &Surface::unknown_properties)
      .def("__str__", [](const Surface &s) { return s.name; })
      .def("__repr__", [](const Surface &s) { return "<Surface: " + s.name + ">"; })
      .def_property_readonly("type", [](const Surface &) { return ReactionType::Surface; });

  py::class_<Photolysis>(m, "Photolysis")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            Photolysis photolysis;
            photolysis.name = name;
            return photolysis;
        }))
            .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
                Photolysis photolysis;
                photolysis.name = name;
    
                // Iterate through the dictionary and set known properties
                for (const auto &[key, value] : properties) {
                    try {
                        if (key == validation::scaling_factor) {
                            photolysis.scaling_factor = value.cast<double>();
                        } else if (key == validation::reactants) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            photolysis.reactants = get_reaction_components(value.cast<py::list>());
                        } else if (key == validation::products) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            photolysis.products = get_reaction_components(value.cast<py::list>());
                        } else if (key == validation::gas_phase) {
                            photolysis.gas_phase = value.cast<Phase>().name;
                        } else {
                            // Add unmatched properties to unknown_properties
                            if (key.rfind("__", 0) != 0) {
                                throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                            }
                            photolysis.unknown_properties[key] = py::str(value);
                        }
                    } catch (const py::cast_error &e) {
                        throw py::value_error("Invalid type for property '" + key + "'.");
                    }
                }
                return photolysis;
            }))
      .def_readwrite("scaling_factor", &Photolysis::scaling_factor)
      .def_readwrite("reactants", &Photolysis::reactants)
      .def_readwrite("products", &Photolysis::products)
      .def_readwrite("name", &Photolysis::name)
      .def_readwrite("gas_phase", &Photolysis::gas_phase)
      .def_readwrite("unknown_properties", &Photolysis::unknown_properties)
      .def("__str__", [](const Photolysis &p) { return p.name; })
      .def("__repr__", [](const Photolysis &p) { return "<Photolysis: " + p.name + ">"; })
      .def_property_readonly("type", [](const Photolysis &) { return ReactionType::Photolysis; });

  py::class_<CondensedPhasePhotolysis>(m, "CondensedPhasePhotolysis")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            CondensedPhasePhotolysis cpp;
            cpp.name = name;
            return cpp;
        }))
            .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
                CondensedPhasePhotolysis cpp;
                cpp.name = name;
    
                // Iterate through the dictionary and set known properties
                for (const auto &[key, value] : properties) {
                    try {
                        if (key == validation::scaling_factor) {
                            cpp.scaling_factor = value.cast<double>();
                        } else if (key == validation::reactants) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            cpp.reactants = get_reaction_components(value.cast<py::list>());
                        } else if (key == validation::products) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            cpp.products = get_reaction_components(value.cast<py::list>());
                        } else if (key == validation::aerosol_phase_water) {
                            cpp.aerosol_phase_water = value.cast<Species>().name;
                        } else if (key == validation::aerosol_phase) {
                            cpp.aerosol_phase = value.cast<Phase>().name;
                        } else {
                            // Add unmatched properties to unknown_properties
                            if (key.rfind("__", 0) != 0) {
                                throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                            }
                            cpp.unknown_properties[key] = py::str(value);
                        }
                    } catch (const py::cast_error &e) {
                        throw py::value_error("Invalid type for property '" + key + "'.");
                    }
                }
                return cpp;
            }))
      .def_readwrite("scaling_factor", &CondensedPhasePhotolysis::scaling_factor)
      .def_readwrite("reactants", &CondensedPhasePhotolysis::reactants)
      .def_readwrite("products", &CondensedPhasePhotolysis::products)
      .def_readwrite("name", &CondensedPhasePhotolysis::name)
      .def_readwrite("aerosol_phase", &CondensedPhasePhotolysis::aerosol_phase)
      .def_readwrite("aerosol_phase_water", &CondensedPhasePhotolysis::aerosol_phase_water)
      .def_readwrite("unknown_properties", &CondensedPhasePhotolysis::unknown_properties)
      .def("__str__", [](const CondensedPhasePhotolysis &cpp) { return cpp.name; })
      .def("__repr__", [](const CondensedPhasePhotolysis &cpp) { return "<CondensedPhasePhotolysis: " + cpp.name + ">"; })
      .def_property_readonly("type", [](const CondensedPhasePhotolysis &) { return ReactionType::CondensedPhasePhotolysis; });

  py::class_<Emission>(m, "Emission")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            Emission emission;
            emission.name = name;
            return emission;
        }))
            .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
                Emission emission;
                emission.name = name;
    
                // Iterate through the dictionary and set known properties
                for (const auto &[key, value] : properties) {
                    try {
                        if (key == validation::scaling_factor) {
                            emission.scaling_factor = value.cast<double>();
                        } else if (key == validation::products) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            emission.products = get_reaction_components(value.cast<py::list>());
                        } else if (key == validation::gas_phase) {
                            emission.gas_phase = value.cast<Phase>().name;
                        } else {
                            // Add unmatched properties to unknown_properties
                            if (key.rfind("__", 0) != 0) {
                                throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                            }
                            emission.unknown_properties[key] = py::str(value);
                        }
                    } catch (const py::cast_error &e) {
                        throw py::value_error("Invalid type for property '" + key + "'.");
                    }
                }
                return emission;
            }))
      .def_readwrite("scaling_factor", &Emission::scaling_factor)
      .def_readwrite("products", &Emission::products)
      .def_readwrite("name", &Emission::name)
      .def_readwrite("gas_phase", &Emission::gas_phase)
      .def_readwrite("unknown_properties", &Emission::unknown_properties)
      .def("__str__", [](const Emission &e) { return e.name; })
      .def("__repr__", [](const Emission &e) { return "<Emission: " + e.name + ">"; })
      .def_property_readonly("type", [](const Emission &) { return ReactionType::Emission; });

  py::class_<FirstOrderLoss>(m, "FirstOrderLoss")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            FirstOrderLoss fol;
            fol.name = name;
            return fol;
        }))
            .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
                FirstOrderLoss fol;
                fol.name = name;
    
                // Iterate through the dictionary and set known properties
                for (const auto &[key, value] : properties) {
                    try {
                        if (key == validation::scaling_factor) {
                            fol.scaling_factor = value.cast<double>();
                        } else if (key == validation::reactants) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            fol.reactants = get_reaction_components(value.cast<py::list>());
                        } else if (key == validation::gas_phase) {
                            fol.gas_phase = value.cast<Phase>().name;
                        } else {
                            // Add unmatched properties to unknown_properties
                            if (key.rfind("__", 0) != 0) {
                                throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                            }
                            fol.unknown_properties[key] = py::str(value);
                        }
                    } catch (const py::cast_error &e) {
                        throw py::value_error("Invalid type for property '" + key + "'.");
                    }
                }
                return fol;
            }))
      .def_readwrite("scaling_factor", &FirstOrderLoss::scaling_factor)
      .def_readwrite("reactants", &FirstOrderLoss::reactants)
      .def_readwrite("name", &FirstOrderLoss::name)
      .def_readwrite("gas_phase", &FirstOrderLoss::gas_phase)
      .def_readwrite("unknown_properties", &FirstOrderLoss::unknown_properties)
      .def("__str__", [](const FirstOrderLoss &fol) { return fol.name; })
      .def("__repr__", [](const FirstOrderLoss &fol) { return "<FirstOrderLoss: " + fol.name + ">"; })
      .def_property_readonly("type", [](const FirstOrderLoss &) { return ReactionType::FirstOrderLoss; });

  py::class_<AqueousEquilibrium>(m, "AqueousEquilibrium")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            AqueousEquilibrium ae;
            ae.name = name;
            return ae;
        }))
            .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
                AqueousEquilibrium ae;
                ae.name = name;
    
                // Iterate through the dictionary and set known properties
                for (const auto &[key, value] : properties) {
                    try {
                        if (key == validation::A) {
                            ae.A = value.cast<double>();
                        } else if (key == validation::C) {
                            ae.C = value.cast<double>();
                        } else if (key == validation::k_reverse) {
                            ae.k_reverse = value.cast<double>();
                        } else if (key == validation::reactants) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            ae.reactants = get_reaction_components(value.cast<py::list>());
                        } else if (key == validation::products) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            ae.products = get_reaction_components(value.cast<py::list>());
                        } else if (key == validation::gas_phase) {
                            ae.gas_phase = value.cast<Phase>().name;
                        } else if (key == validation::aerosol_phase) {
                            ae.aerosol_phase = value.cast<Phase>().name;
                        } else if (key == validation::aerosol_phase_water) {
                            ae.aerosol_phase_water = value.cast<Species>().name;
                        } else {
                            // Add unmatched properties to unknown_properties
                            if (key.rfind("__", 0) != 0) {
                                throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                            }
                            ae.unknown_properties[key] = py::str(value);
                        }
                    } catch (const py::cast_error &e) {
                        throw py::value_error("Invalid type for property '" + key + "'.");
                    }
                }
                return ae;
            }))
      .def_readwrite("name", &AqueousEquilibrium::name)
      .def_readwrite("gas_phase", &AqueousEquilibrium::gas_phase)
      .def_readwrite("aerosol_phase", &AqueousEquilibrium::aerosol_phase)
      .def_readwrite("aerosol_phase_water", &AqueousEquilibrium::aerosol_phase_water)
      .def_readwrite("reactants", &AqueousEquilibrium::reactants)
      .def_readwrite("products", &AqueousEquilibrium::products)
      .def_readwrite("A", &AqueousEquilibrium::A)
      .def_readwrite("C", &AqueousEquilibrium::C)
      .def_readwrite("k_reverse", &AqueousEquilibrium::k_reverse)
      .def_readwrite("unknown_properties", &AqueousEquilibrium::unknown_properties)
      .def("__str__", [](const AqueousEquilibrium &ae) { return ae.name; })
      .def("__repr__", [](const AqueousEquilibrium &ae) { return "<AqueousEquilibrium: " + ae.name + ">"; })
      .def_property_readonly("type", [](const AqueousEquilibrium &) { return ReactionType::AqueousEquilibrium; });

  py::class_<WetDeposition>(m, "WetDeposition")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            WetDeposition wd;
            wd.name = name;
            return wd;
        }))
            .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
                WetDeposition wd;
                wd.name = name;
    
                // Iterate through the dictionary and set known properties
                for (const auto &[key, value] : properties) {
                    try {
                        if (key == validation::scaling_factor) {
                            wd.scaling_factor = value.cast<double>();
                        } else if (key == validation::aerosol_phase) {
                            wd.aerosol_phase = value.cast<Phase>().name;
                        } else {
                            // Add unmatched properties to unknown_properties
                            if (key.rfind("__", 0) != 0) {
                                throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                            }
                            wd.unknown_properties[key] = py::str(value);
                        }
                    } catch (const py::cast_error &e) {
                        throw py::value_error("Invalid type for property '" + key + "'.");
                    }
                }
                return wd;
            }))
      .def_readwrite("scaling_factor", &WetDeposition::scaling_factor)
      .def_readwrite("name", &WetDeposition::name)
      .def_readwrite("aerosol_phase", &WetDeposition::aerosol_phase)
      .def_readwrite("unknown_properties", &WetDeposition::unknown_properties)
      .def("__str__", [](const WetDeposition &wd) { return wd.name; })
      .def("__repr__", [](const WetDeposition &wd) { return "<WetDeposition: " + wd.name + ">"; })
      .def_property_readonly("type", [](const WetDeposition &) { return ReactionType::WetDeposition; });

  py::class_<HenrysLaw>(m, "HenrysLaw")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            HenrysLaw hl;
            hl.name = name;
            return hl;
        }))
            .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
                HenrysLaw hl;
                hl.name = name;
    
                // Iterate through the dictionary and set known properties
                for (const auto &[key, value] : properties) {
                    try {
                        if (key == validation::gas_phase) {
                            hl.gas_phase = value.cast<Phase>().name;
                        } else if (key == validation::gas_phase_species) {
                            hl.gas_phase_species = ReactionComponent(value.cast<Species>().name);
                        } else if (key == validation::aerosol_phase) {
                            hl.aerosol_phase = value.cast<Phase>().name;
                        } else if (key == validation::aerosol_phase_water) {
                            hl.aerosol_phase_water = ReactionComponent(value.cast<Species>().name);
                        } else if (key == validation::aerosol_phase_species) {
                            hl.aerosol_phase_species = ReactionComponent(value.cast<Species>().name);
                        } else {
                            // Add unmatched properties to unknown_properties
                            if (key.rfind("__", 0) != 0) {
                                throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                            }
                            hl.unknown_properties[key] = py::str(value);
                        }
                    } catch (const py::cast_error &e) {
                        throw py::value_error("Invalid type for property '" + key + "'.");
                    }
                }
                return hl;
            }))
      .def_readwrite("name", &HenrysLaw::name)
      .def_readwrite("gas_phase", &HenrysLaw::gas_phase)
      .def_readwrite("gas_phase_species", &HenrysLaw::gas_phase_species)
      .def_readwrite("aerosol_phase", &HenrysLaw::aerosol_phase)
      .def_readwrite("aerosol_phase_water", &HenrysLaw::aerosol_phase_water)
      .def_readwrite("aerosol_phase_species", &HenrysLaw::aerosol_phase_species)
      .def_readwrite("unknown_properties", &HenrysLaw::unknown_properties)
      .def("__str__", [](const HenrysLaw &hl) { return hl.name; })
      .def("__repr__", [](const HenrysLaw &hl) { return "<HenrysLaw: " + hl.name + ">"; })
      .def_property_readonly("type", [](const HenrysLaw &) { return ReactionType::HenrysLaw; });

  py::class_<SimpolPhaseTransfer>(m, "SimpolPhaseTransfer")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            SimpolPhaseTransfer spt;
            spt.name = name;
            return spt;
        }))
            .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
                SimpolPhaseTransfer spt;
                spt.name = name;
    
                // Iterate through the dictionary and set known properties
                for (const auto &[key, value] : properties) {
                    try {
                        if (key == validation::B) {
                            auto B_list = value.cast<py::list>();
                            if (B_list.size() != 4) {
                                throw py::value_error("Invalid size for property '" + key + "'. Expected a list of size 4.");
                            }
                            spt.B = std::array<double, 4>{B_list[0].cast<double>(), B_list[1].cast<double>(), B_list[2].cast<double>(), B_list[3].cast<double>()};
                        } else if (key == validation::gas_phase) {
                            spt.gas_phase = value.cast<Phase>().name;
                        } else if (key == validation::gas_phase_species) {
                            spt.gas_phase_species = ReactionComponent(value.cast<Species>().name);
                        } else if (key == validation::aerosol_phase) {
                            spt.aerosol_phase = value.cast<Phase>().name;
                        } else if (key == validation::aerosol_phase_species) {
                            spt.aerosol_phase_species = ReactionComponent(value.cast<Species>().name);
                        } else {
                            // Add unmatched properties to unknown_properties
                            if (key.rfind("__", 0) != 0) {
                                throw py::value_error("Unknown property '" + key + "' must start with '__'.");
                            }
                            spt.unknown_properties[key] = py::str(value);
                        }
                    } catch (const py::cast_error &e) {
                        throw py::value_error("Invalid type for property '" + key + "'.");
                    }
                }
                return spt;
            }))
      .def_readwrite("gas_phase", &SimpolPhaseTransfer::gas_phase)
      .def_readwrite("gas_phase_species", &SimpolPhaseTransfer::gas_phase_species)
      .def_readwrite("aerosol_phase", &SimpolPhaseTransfer::aerosol_phase)
      .def_readwrite("aerosol_phase_species", &SimpolPhaseTransfer::aerosol_phase_species)
      .def_readwrite("name", &SimpolPhaseTransfer::name)
      .def_readwrite("B", &SimpolPhaseTransfer::B)
      .def_readwrite("unknown_properties", &SimpolPhaseTransfer::unknown_properties)
      .def("__str__", [](const SimpolPhaseTransfer &spt) { return spt.name; })
      .def("__repr__", [](const SimpolPhaseTransfer &spt) { return "<SimpolPhaseTransfer: " + spt.name + ">"; })
      .def_property_readonly("type", [](const SimpolPhaseTransfer &) { return ReactionType::SimpolPhaseTransfer; });

  py::class_<Reactions>(m, "Reactions")
      .def(py::init<>())
      .def_readwrite("arrhenius", &Reactions::arrhenius)
      .def_readwrite("branched", &Reactions::branched)
      .def_readwrite("condensed_phase_arrhenius", &Reactions::condensed_phase_arrhenius)
      .def_readwrite("condensed_phase_photolysis", &Reactions::condensed_phase_photolysis)
      .def_readwrite("emission", &Reactions::emission)
      .def_readwrite("first_order_loss", &Reactions::first_order_loss)
      .def_readwrite("simpol_phase_transfer", &Reactions::simpol_phase_transfer)
      .def_readwrite("aqueous_equilibrium", &Reactions::aqueous_equilibrium)
      .def_readwrite("wet_deposition", &Reactions::wet_deposition)
      .def_readwrite("henrys_law", &Reactions::henrys_law)
      .def_readwrite("photolysis", &Reactions::photolysis)
      .def_readwrite("surface", &Reactions::surface)
      .def_readwrite("troe", &Reactions::troe)
      .def_readwrite("tunneling", &Reactions::tunneling)
      .def(
          "__len__",
          [](const Reactions &r)
          {
            return r.arrhenius.size() + r.branched.size() + r.condensed_phase_arrhenius.size() + r.condensed_phase_photolysis.size() +
                   r.emission.size() + r.first_order_loss.size() + r.simpol_phase_transfer.size() + r.aqueous_equilibrium.size() +
                   r.wet_deposition.size() + r.henrys_law.size() + r.photolysis.size() + r.surface.size() + r.troe.size() + r.tunneling.size();
          })
      .def("__str__", [](const Reactions &r) { return "Reactions"; })
      .def("__repr__", [](const Reactions &r) { return "<Reactions>"; })
      .def("__iter__", [](Reactions &r) { return ReactionsIterator(r); });

  py::class_<ReactionsIterator>(m, "ReactionsIterator")
      .def("__iter__", [](ReactionsIterator &it) -> ReactionsIterator & { return it; })
      .def("__next__", &ReactionsIterator::next);

  py::class_<Mechanism>(m, "Mechanism")
      .def(py::init<>())
        .def(py::init([](const std::string &name) {
            Mechanism mechanism;
            mechanism.name = name;
            return mechanism;
        }))
            .def(py::init([](const std::string &name, const std::map<std::string, py::object> &properties) {
                Mechanism mechanism;
                mechanism.name = name;
    
                // Iterate through the dictionary and set known properties
                for (const auto &[key, value] : properties) {
                    try {
                        if (key == validation::species) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            for (auto &item : value.cast<py::list>()) {
                                if (!py::isinstance<Species>(item)) {
                                    throw py::value_error("Invalid type for property '" + key + "'. Expected a list of Species.");
                                }
                                mechanism.species.push_back(item.cast<Species>());
                            }
                        } else if (key == validation::phases) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            for (auto &item : value.cast<py::list>()) {
                                if (!py::isinstance<Phase>(item)) {
                                    throw py::value_error("Invalid type for property '" + key + "'. Expected a list of Phase.");
                                }
                                mechanism.phases.push_back(item.cast<Phase>());
                            }
                        } else if (key == validation::reactions) {
                            if (!py::isinstance<py::list>(value)) {
                                throw py::value_error("Invalid type for property '" + key + "'. Expected a list.");
                            }
                            mechanism.reactions = create_reactions(value.cast<py::list>());
                        } else if (key == validation::version) {
                            mechanism.version = value.cast<mechanism_configuration::Version>();
                        } else {
                           throw py::value_error("Unknown mechanism component '" + key + ".");
                        }
                    } catch (const py::cast_error &e) {
                        throw py::value_error("Invalid type for mechanism component '" + key + "'.");
                    }
                }
                return mechanism;
            }))
      .def_readwrite("name", &Mechanism::name)
      .def_readwrite("species", &Mechanism::species)
      .def_readwrite("phases", &Mechanism::phases)
      .def_readwrite("reactions", &Mechanism::reactions)
      .def_readwrite("version", &Mechanism::version)
      .def("__str__", [](const Mechanism &m) { return m.name; })
      .def("__repr__", [](const Mechanism &m) { return "<Mechanism: " + m.name + ">"; });

  py::class_<mechanism_configuration::Version>(m, "Version")
      .def(py::init<>())
      .def(py::init<unsigned int, unsigned int, unsigned int>())
      .def(py::init<std::string>())
      .def_readwrite("major", &mechanism_configuration::Version::major)
      .def_readwrite("minor", &mechanism_configuration::Version::minor)
      .def_readwrite("patch", &mechanism_configuration::Version::patch)
      .def("to_string", &mechanism_configuration::Version::to_string)
      .def("__str__", &mechanism_configuration::Version::to_string)
      .def("__repr__", [](const mechanism_configuration::Version &v) { return "<Version: " + v.to_string() + ">"; });

  using V1Parser = mechanism_configuration::v1::Parser;

  py::class_<V1Parser>(m, "Parser")
      .def(py::init<>())
      .def(
          "parse",
          [](V1Parser &self, const std::string &path)
          {
            auto parsed = self.Parse(std::filesystem::path(path));
            if (parsed)
            {
              return *parsed;
            }
            else
            {
              std::string error = "Error parsing file: " + path + "\n";
              for (auto &e : parsed.errors)
              {
                error += e.second + "\n";
              }
              throw std::runtime_error(error);
            }
          });
}

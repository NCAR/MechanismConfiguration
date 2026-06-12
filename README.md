# Chemical Mechanism Configurations

An attempt at defining a model-independent configuration schema for atmospheric chemical systems in JSON and YAML.

[![GitHub Releases](https://img.shields.io/github/release/ncar/MechanismConfiguration.svg)](https://github.com/ncar/MechanismConfiguration/releases)
[![License](https://img.shields.io/github/license/ncar/MechanismConfiguration.svg)](https://github.com/ncar/MechanismConfiguration/blob/master/LICENSE)
[![Docs build](https://github.com/ncar/MechanismConfiguration/actions/workflows/gh-pages.yml/badge.svg)](https://github.com/ncar/MechanismConfiguration/actions/workflows/gh-pages.yml)
[![Windows](https://github.com/ncar/MechanismConfiguration/actions/workflows/windows.yml/badge.svg)](https://github.com/ncar/MechanismConfiguration/actions/workflows/windows.yml)
[![Mac](https://github.com/ncar/MechanismConfiguration/actions/workflows/mac.yml/badge.svg)](https://github.com/ncar/MechanismConfiguration/actions/workflows/mac.yml)
[![Ubuntu](https://github.com/ncar/MechanismConfiguration/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/ncar/MechanismConfiguration/actions/workflows/ubuntu.yml)
[![codecov](https://codecov.io/gh/NCAR/MechanismConfiguration/branch/main/graph/badge.svg)](https://codecov.io/gh/NCAR/MechanismConfiguration)
[![DOI](https://zenodo.org/badge/668458983.svg)](https://doi.org/10.5281/zenodo.15116380)

Copyright (C) 2017–2026 University Corporation for Atmospheric Research, University of Illinois at Urbana–Champaign, Barcelona Supercomputing Center

The configuration documentation can be found [here](https://ncar.github.io/MechanismConfiguration/).

## Usage

Everything goes through the canonical `mechanism_configuration::Mechanism`. You can either
**parse** a configuration file (the version is detected and dispatched automatically), or build
a `Mechanism` in code and **validate** it. `validate()` runs the same semantic checks the parser
uses, so it applies to any mechanism regardless of where it came from.

```cpp
#include <mechanism_configuration/mechanism_configuration.hpp>

#include <iostream>

using namespace mechanism_configuration;

void print_errors(const Errors& errors)
{
  for (const auto& [code, message] : errors)
    std::cerr << "  [" << ErrorCodeToString(code) << "] " << message << '\n';
}

int main()
{
  // 1) Parse from a file (YAML or JSON; v0 or v1). Returns std::expected<Mechanism, Errors>
  //    with both structural and semantic errors reported.
  if (auto parsed = parse("examples/v1/full_configuration.yaml"))
  {
    const Mechanism& mechanism = *parsed;
    std::cout << "Parsed '" << mechanism.name << "': " << mechanism.species.size()
              << " species, " << mechanism.reactions.arrhenius.size() << " Arrhenius reactions\n";
  }
  else
  {
    std::cerr << "Failed to parse file:\n";
    print_errors(parsed.error());
  }

  // 2) Build a Mechanism in code and validate it (species exist, reactants are in their
  //    phase, no duplicate names, ...).
  Mechanism mechanism;
  mechanism.name = "example";
  mechanism.species = { { .name = "A" }, { .name = "B" } };
  mechanism.phases = { { .name = "gas", .species = { { .name = "A" }, { .name = "B" } } } };

  types::Arrhenius reaction;
  reaction.name = "A -> B";
  reaction.gas_phase = "gas";
  reaction.reactants = { { .name = "A" } };  // reactants must be registered in the phase
  reaction.products = { { .name = "B" } };   // products may reference any phase
  mechanism.reactions.arrhenius = { reaction };

  if (Errors errors = validate(mechanism); errors.empty())
    std::cout << "In-code mechanism is valid\n";
  else
  {
    std::cerr << "In-code mechanism is invalid:\n";
    print_errors(errors);
  }

  return 0;
}
```

## Building the Documentation

With python and pip installed, go to the `docs/` folder and run:
```
pip install -r requirements.txt
make html
```

Then, you can navigate to `docs/build/html/index.html` in a browser to view the documentation.
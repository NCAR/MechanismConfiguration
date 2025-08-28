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

Copyright (C) 2017–2025 University Corporation for Atmospheric Research, University of Illinois at Urbana–Champaign, Barcelona Supercomputing Center

The configuration documentation can be found [here](https://ncar.github.io/MechanismConfiguration/).

## Building the Documentation

With python and pip installed, go to the `docs/` folder and run:
```
pip install -r requirements.txt
make html
```

Then, you can navigate to `docs/build/html/index.html` in a browser to view the documentation.
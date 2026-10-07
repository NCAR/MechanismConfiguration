##########
User Guide
##########

A mechanism configuration is a JSON or YAML file that describes the species, phases,
and reactions of a chemical system. The ``version`` key at the top of the file tells
the parser which format the file uses.

- **Version 1** is the current format. All values are in SI units. Use this format for new mechanisms.
- **Version 0** is the legacy CAMP format. The parser still reads it, but no new features are added to it.

The ``examples`` directory of the repository has complete configurations for each format.
The tests parse each of these examples.

If the examples do not show what you need, please
`fill out an issue <https://github.com/NCAR/MechanismConfiguration/issues/new>`_.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   /v1/index
   /v0/index

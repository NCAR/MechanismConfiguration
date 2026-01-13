########
Overview
########

Mechanism configurations are contained in a single JSON or YAML file.

At the highest level, the configuration file contains information about
the name of the mechanism, the version of Mechanism Configuration
being used, and the three main components of the mechanism:

.. tab-set::

  .. tab-item:: YAML

    .. code-block:: yaml

        version: 1.0.0
        name: My Mechanism
        species:
          - ...
        phases:
          - ...
        reactions:
          - ...

  .. tab-item:: JSON

    .. code-block:: json

      {
        "version": "1.0.0",
        "name": "My Mechanism",
        "species": [
          "..."
        ],
        "phases": [
          "..."
        ],
        "reactions": [
          "..."
        ]
      }




The three main components of a mechanism configuration are described here:

:ref:`v1-chemical-species`

:ref:`v1-phases`

:ref:`v1-reactions`

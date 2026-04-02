########
Overview
########

Mechanism configurations are contained in a single JSON or YAML file.

At the highest level, the configuration file contains information about
the name of the mechanism, the version of Mechanism Configuration
being used, and the three main components of the mechanism:
``species``, ``phases``, and ``reactions``.

Each of these three sections can be written in one of two formats:

- **Inline** — the content is listed directly in the main configuration file (version ``1.0.x``)
- **File-list** — the content is split across one or more separate files referenced by a ``files`` key (version ``1.1.x``)

Both formats can be mixed freely within the same configuration file.
For example, ``species`` can be inline while ``reactions`` references external files.

.. note::
   File-list format requires minor version ``1`` or greater (e.g. ``1.1.0``).
   Inline-only configurations use minor version ``0`` (e.g. ``1.0.0``).

Inline format
=============

All sections are defined directly in the main file. Use version ``1.0.0``.

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
        "species": [ "..." ],
        "phases": [ "..." ],
        "reactions": [ "..." ]
      }

File-list format
================

Each section references one or more external files. The files are resolved
relative to the main configuration file. Multiple files are merged in order.
Use version ``1.1.0``.

.. tab-set::

  .. tab-item:: YAML

    .. code-block:: yaml

        version: 1.1.0
        name: My Mechanism
        species:
          files:
            - species.yaml
        phases:
          files:
            - gas_phase.yaml
        reactions:
          files:
            - troposphere.yaml
            - stratosphere.yaml

  .. tab-item:: JSON

    .. code-block:: json

      {
        "version": "1.1.0",
        "name": "My Mechanism",
        "species": { "files": ["species.json"] },
        "phases":  { "files": ["gas_phase.json"] },
        "reactions": {
          "files": ["troposphere.json", "stratosphere.json"]
        }
      }

Mixed format
============

Sections can independently use inline or file-list format in the same file.
Use version ``1.1.0`` when any section is a file-list.

.. tab-set::

  .. tab-item:: JSON

    .. code-block:: json

      {
        "version": "1.1.0",
        "name": "My Mechanism",
        "species": [
          { "name": "A" },
          { "name": "B" }
        ],
        "phases": { "files": ["gas_phase.json"] },
        "reactions": { "files": ["reactions.json"] }
      }

The three main components of a mechanism configuration are described here:

:ref:`v1-chemical-species`

:ref:`v1-phases`

:ref:`v1-reactions`

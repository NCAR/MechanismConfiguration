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

.. _v1-versions:

Versions
========

Each minor version of the v1 format adds features. A configuration must declare a
version that has all of the features that it uses. If it does not, the parser reports
an ``InvalidVersion`` error. A newer minor version can still use all of the features
of the older minor versions.

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Version
     - Adds
   * - ``1.0.0``
     - The inline format. All values are in SI units.
   * - ``1.1.0``
     - The file-list format
   * - ``1.2.0``
     - The ``"aerosol representations"`` and ``"aerosol processes"`` sections. See :ref:`v1-aerosol`.
   * - ``1.3.0``
     - The ``emissions`` section. See :ref:`v1-emissions`.

The newest version that this library supports is ``1.3.0``. The parser reports an
``InvalidVersion`` error for a newer minor version. The ``examples/v1`` directory of the
repository has a complete example for each version. See :doc:`examples/index`.

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

  .. tab-item:: YAML

    .. code-block:: yaml

        version: 1.1.0
        name: My Mechanism
        species:
          - name: A
          - name: B
        phases:
          files:
            - gas_phase.yaml
        reactions:
          files:
            - reactions.yaml

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

A configuration can also have these optional top-level sections:

- ``"aerosol representations"`` and ``"aerosol processes"``. See :ref:`v1-aerosol`.
- ``emissions``. See :ref:`v1-emissions`.

The aerosol sections can use the inline or the file-list format. The ``emissions`` section must be inline.

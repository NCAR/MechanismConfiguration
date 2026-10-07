.. _v1-emissions:

#########
Emissions
#########

The ``emissions`` section tells an emissions model which inventory files to read, and how
to put the inventory species into the mechanism species. MUSICA reads this section for
:doc:`MIEM <miem:index>`. The section is optional.

The ``emissions`` section is different from the :doc:`../reactions/emission` reaction.
The ``EMISSION`` reaction adds a species at a rate that the host model gives.

The section has four parts:

- ``inventories``: the inventory files to read
- ``"species maps"``: how inventory species become mechanism species
- ``regridding``: how to put the inventory grid on the model grid
- ``sources``: each source joins one inventory to one species map

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         emissions:
           inventories:
             - name: cams bc
               directory: cams
               file pattern: CAMS-GLOB-ANT_{YYYY}-{MM}.nc
               convention: uptempo
               molecular weights:
                 bc_anth_sum: 0.012
           species maps:
             - name: bc map
               mappings:
                 - inventory species: bc_anth_sum
                   mechanism species: BC
                   scaling factor: 1.0
           regridding:
             type: none
           sources:
             - name: CAMS black carbon
               mode: offline
               type: anthropogenic
               inventory: cams bc
               species map: bc map
               temporal interpolation: linear
               vertical injection: surface
               category: 0
               hierarchy: 1
               scaling factor: 1.0
               sector: anthropogenic
               __notes: a custom property

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "emissions": {
             "inventories": [
               {
                 "name": "cams bc",
                 "directory": "cams",
                 "file pattern": "CAMS-GLOB-ANT_{YYYY}-{MM}.nc",
                 "convention": "uptempo",
                 "molecular weights": { "bc_anth_sum": 0.012 }
               }
             ],
             "species maps": [
               {
                 "name": "bc map",
                 "mappings": [
                   {
                     "inventory species": "bc_anth_sum",
                     "mechanism species": "BC",
                     "scaling factor": 1.0
                   }
                 ]
               }
             ],
             "regridding": { "type": "none" },
             "sources": [
               {
                 "name": "CAMS black carbon",
                 "mode": "offline",
                 "type": "anthropogenic",
                 "inventory": "cams bc",
                 "species map": "bc map",
                 "temporal interpolation": "linear",
                 "vertical injection": "surface",
                 "category": 0,
                 "hierarchy": 1,
                 "scaling factor": 1.0,
                 "sector": "anthropogenic",
                 "__notes": "a custom property"
               }
             ]
           }
         }

Inventories
===========

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``name``
     - Yes
     - The name of the inventory. Sources use this name. It must be unique.
   * - ``directory``
     - Yes
     - The directory that holds the inventory files
   * - ``"file pattern"``
     - Yes
     - The file name pattern. It can contain the ``{YYYY}``, ``{MM}``, ``{DD}``, and ``{HH}`` date tokens.
   * - ``convention``
     - Yes
     - The format convention of the inventory files (for example ``uptempo`` or ``eccad``).
       See :doc:`MIEM inventory conventions <miem:user_guide/inventory_conventions>`.
   * - ``"molecular weights"``
     - No
     - A map from inventory species name to molecular weight (kg mol\ :sup:`-1`). Use it when the
       file gives a molar or number flux (for example molecules m\ :sup:`-2` s\ :sup:`-1`) instead of a mass flux.

Species Maps
============

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``name``
     - Yes
     - The name of the species map. Sources use this name. It must be unique.
   * - ``mappings``
     - Yes
     - A list of mappings. Each mapping has the keys in the next table.

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Mapping key
     - Required
     - Description
   * - ``"inventory species"``
     - Yes
     - The species name in the inventory file
   * - ``"mechanism species"``
     - Yes
     - The species name in the mechanism
   * - ``"scaling factor"``
     - No
     - The fraction of the inventory species that goes to the mechanism species. The default is 1.0.

One inventory species can map to more than one mechanism species. For each inventory species
in a species map, the sum of the scaling factors must not be more than 1.0.

Regridding
==========

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``type``
     - No
     - ``none``. This is the only supported value.

Sources
=======

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``name``
     - Yes
     - The name of the source. It must be unique.
   * - ``mode``
     - Yes
     - ``offline``. This is the only supported value. The parser reports an error for ``online``.
   * - ``type``
     - Yes
     - ``anthropogenic``, ``fire``, ``biogenic``, ``dust``, ``sea salt``, or ``lightning``
   * - ``inventory``
     - Yes
     - The name of an inventory in ``inventories``
   * - ``"species map"``
     - Yes
     - The name of a species map in ``"species maps"``
   * - ``"temporal interpolation"``
     - No
     - ``linear``, ``nearest``, or ``none``. The default is ``linear``.
   * - ``"vertical injection"``
     - No
     - ``surface``. This is the only supported value. The parser reports an error for ``plume``.
   * - ``category``
     - No
     - The emissions model adds the sources in different categories. The default is 0.
   * - ``hierarchy``
     - No
     - In one category, the source with the highest hierarchy is used. The default is 1.
   * - ``"scaling factor"``
     - No
     - A scaling factor for all of the emissions of this source. The default is 1.0.
   * - ``sector``
     - No
     - A sector name for diagnostics

Each pair of ``category`` and ``hierarchy`` must be unique across all sources.
A source can also have custom properties that start with two underscores (for example ``__notes``).

Validation
==========

The parser and ``Validate()`` check these rules:

- The inventory names, species map names, and source names are unique.
- The inventory and species map of each source exist.
- Each pair of ``category`` and ``hierarchy`` is unique.
- For each inventory species in a species map, the sum of the scaling factors is not more than 1.0.

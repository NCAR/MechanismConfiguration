.. _v1-aerosol-representations:

###############
Representations
###############

A representation describes how a particle population is distributed in size, and 
what parameters are tracked for that population.
Each representation holds one or more phases and can use the species in those phases.
All representations are assumed to be internally mixed.

All representations have these keys:

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``type``
     - Yes
     - ``UNIFORM_SECTION``, ``SINGLE_MOMENT_MODE``, or ``TWO_MOMENT_MODE``
   * - ``name``
     - Yes
     - The name of the representation
   * - ``phases``
     - Yes
     - A list of phase names. Each phase must exist in the top-level ``phases`` list.

For the equations of each representation, see the
:doc:`MIAM representations guide <miam:user_guide/representations>`.

Uniform Section
===============

A sectional bin with a fixed minimum and maximum radius.

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         type: UNIFORM_SECTION
         name: dust
         phases: [ organic ]
         "minimum radius [m]": 1.0e-7
         "maximum radius [m]": 1.0e-6

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "type": "UNIFORM_SECTION",
           "name": "dust",
           "phases": [ "organic" ],
           "minimum radius [m]": 1.0e-7,
           "maximum radius [m]": 1.0e-6
         }

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``"minimum radius [m]"``
     - Yes
     - The minimum particle radius of the section
   * - ``"maximum radius [m]"``
     - Yes
     - The maximum particle radius of the section

Single Moment Mode
==================

A log-normal mode with a fixed geometric mean radius and geometric standard deviation.
MIAM calculates the number concentration from the total species volume.

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         type: SINGLE_MOMENT_MODE
         name: cloud
         phases: [ aqueous ]
         "geometric mean radius [m]": 1.0e-6
         "geometric standard deviation": 1.4

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "type": "SINGLE_MOMENT_MODE",
           "name": "cloud",
           "phases": [ "aqueous" ],
           "geometric mean radius [m]": 1.0e-6,
           "geometric standard deviation": 1.4
         }

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``"geometric mean radius [m]"``
     - Yes
     - The geometric mean radius of the mode
   * - ``"geometric standard deviation"``
     - Yes
     - The geometric standard deviation of the mode (unitless)

Two Moment Mode
===============

A log-normal mode with a fixed geometric standard deviation. The number concentration
is a state variable of the solver. MIAM calculates the radius from the total species
volume and the number concentration.

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         type: TWO_MOMENT_MODE
         name: aitken
         phases: [ aqueous ]
         "geometric standard deviation": 1.2

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "type": "TWO_MOMENT_MODE",
           "name": "aitken",
           "phases": [ "aqueous" ],
           "geometric standard deviation": 1.2
         }

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``"geometric standard deviation"``
     - Yes
     - The geometric standard deviation of the mode (unitless)

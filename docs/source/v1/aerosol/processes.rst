.. _v1-aerosol-processes:

#########
Processes
#########

Processes change species concentrations over time. Put each process in the
``"aerosol processes"`` list. The ``type`` key selects the process.

For the equations of each process, see the
:doc:`MIAM processes guide <miam:user_guide/processes>`.

Henry's Law Phase Transfer
==========================

The mass transfer of a species between the gas phase and a condensed phase:

.. math::

   \mathrm{A(gas)} \rightleftharpoons \mathrm{A(condensed)}

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         type: HENRYS_LAW_PHASE_TRANSFER
         "gas phase": gas
         "gas-phase species": A
         "condensed phase": aqueous
         "condensed-phase species": A
         solvent: H2O
         "Henry's law constant":
           "HLC_ref [mol m-3 Pa-1]": 1.0e-2
           "C [K]": 3000.0
         "accommodation coefficient": 0.1

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "type": "HENRYS_LAW_PHASE_TRANSFER",
           "gas phase": "gas",
           "gas-phase species": "A",
           "condensed phase": "aqueous",
           "condensed-phase species": "A",
           "solvent": "H2O",
           "Henry's law constant": {
             "HLC_ref [mol m-3 Pa-1]": 1.0e-2,
             "C [K]": 3000.0
           },
           "accommodation coefficient": 0.1
         }

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``"gas phase"``
     - Yes
     - The name of the gas phase
   * - ``"gas-phase species"``
     - Yes
     - The species in the gas phase. It must have a ``"diffusion coefficient [m2 s-1]"`` in the gas phase.
   * - ``"condensed phase"``
     - Yes
     - The name of the condensed phase
   * - ``"condensed-phase species"``
     - Yes
     - The species in the condensed phase
   * - ``solvent``
     - Yes
     - The solvent species in the condensed phase
   * - ``"Henry's law constant"``
     - Yes
     - See :ref:`v1-aerosol-rate-constants`
   * - ``"accommodation coefficient"``
     - Yes
     - The mass accommodation coefficient (unitless)

Dissolved Reaction
==================

An irreversible reaction in a condensed phase:

.. math::

   \mathrm{Reactants} \rightarrow \mathrm{Products}

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         type: DISSOLVED_REACTION
         "condensed phase": aqueous
         solvent: H2O
         reactants:
           - name: A
         products:
           - name: B
         "rate constant":
           type: ARRHENIUS
           A: 1.0e3
           C: 100.0

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "type": "DISSOLVED_REACTION",
           "condensed phase": "aqueous",
           "solvent": "H2O",
           "reactants": [ { "name": "A" } ],
           "products": [ { "name": "B" } ],
           "rate constant": { "type": "ARRHENIUS", "A": 1.0e3, "C": 100.0 }
         }

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``"condensed phase"``
     - Yes
     - The phase that the reaction takes place in
   * - ``solvent``
     - Yes
     - The solvent species in the condensed phase
   * - ``reactants``
     - Yes
     - A list of reactants. Each reactant has a ``name`` and an optional ``coefficient``.
   * - ``products``
     - Yes
     - A list of products. Each product has a ``name`` and an optional ``coefficient``.
   * - ``"rate constant"``
     - Yes
     - An Arrhenius or equilibrium constant. See :ref:`v1-aerosol-rate-constants`.

Dissolved Reversible Reaction
=============================

A reversible reaction in a condensed phase:

.. math::

   \mathrm{Reactants} \rightleftharpoons \mathrm{Products}, \qquad K_\mathrm{eq} = \frac{k_f}{k_r}

Give two of the three constants. MIAM calculates the third constant from the other two.

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         type: DISSOLVED_REVERSIBLE_REACTION
         "condensed phase": aqueous
         solvent: H2O
         reactants:
           - name: A
         products:
           - name: B
         "equilibrium constant":
           A: 1.14e-2
           "C [K]": 2300.0
         "reverse rate constant":
           A: 1.0e3
           C: 100.0

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "type": "DISSOLVED_REVERSIBLE_REACTION",
           "condensed phase": "aqueous",
           "solvent": "H2O",
           "reactants": [ { "name": "A" } ],
           "products": [ { "name": "B" } ],
           "equilibrium constant": { "A": 1.14e-2, "C [K]": 2300.0 },
           "reverse rate constant": { "A": 1.0e3, "C": 100.0 }
         }

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``"condensed phase"``
     - Yes
     - The phase that the reaction takes place in
   * - ``solvent``
     - Yes
     - The solvent species in the condensed phase
   * - ``reactants``
     - Yes
     - A list of reactants. Each reactant has a ``name`` and an optional ``coefficient``.
   * - ``products``
     - Yes
     - A list of products. Each product has a ``name`` and an optional ``coefficient``.
   * - ``"forward rate constant"``
     - No
     - :math:`k_f`, an Arrhenius or equilibrium constant. See :ref:`v1-aerosol-rate-constants`.
   * - ``"reverse rate constant"``
     - No
     - :math:`k_r`, an Arrhenius or equilibrium constant. See :ref:`v1-aerosol-rate-constants`.
   * - ``"equilibrium constant"``
     - No
     - :math:`K_\mathrm{eq}`, an equilibrium constant. See :ref:`v1-aerosol-rate-constants`.

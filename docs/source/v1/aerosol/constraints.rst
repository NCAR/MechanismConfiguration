.. _v1-aerosol-constraints:

###########
Constraints
###########

A constraint is an algebraic equation that the solver keeps true at each time step.
Each constraint sets the concentration of one species, the *algebraic species*.
The solver does not integrate that species over time.

Put each constraint in the ``"aerosol processes"`` list, together with the processes.
The ``type`` key selects the constraint.

Henry's Law Equilibrium
=======================

The gas-phase and condensed-phase concentrations of a species are always in Henry's law equilibrium.
The condensed-phase species is the algebraic species.

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         type: HENRYS_LAW_EQUILIBRIUM
         "gas phase": gas
         "gas-phase species": A
         "condensed phase": aqueous
         "condensed-phase species": A
         solvent: H2O
         "Henry's law constant":
           "HLC_ref [mol m-3 Pa-1]": 1.0e-2
           "C [K]": 3000.0

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "type": "HENRYS_LAW_EQUILIBRIUM",
           "gas phase": "gas",
           "gas-phase species": "A",
           "condensed phase": "aqueous",
           "condensed-phase species": "A",
           "solvent": "H2O",
           "Henry's law constant": {
             "HLC_ref [mol m-3 Pa-1]": 1.0e-2,
             "C [K]": 3000.0
           }
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
     - The species in the gas phase
   * - ``"condensed phase"``
     - Yes
     - The name of the condensed phase
   * - ``"condensed-phase species"``
     - Yes
     - The species in the condensed phase
   * - ``solvent``
     - Yes
     - The solvent species. It must have a ``"molecular weight [kg mol-1]"`` in the ``species`` list,
       and a ``"density [kg m-3]"`` in the condensed phase.
   * - ``"Henry's law constant"``
     - Yes
     - See :ref:`v1-aerosol-rate-constants`

Dissolved Equilibrium
=====================

The reactants and products of a reaction in a condensed phase are always in equilibrium.

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         type: DISSOLVED_EQUILIBRIUM
         "condensed phase": aqueous
         "algebraic species": B
         solvent: H2O
         reactants:
           - name: A
         products:
           - name: B
         "equilibrium constant":
           A: 1.14e-2
           "C [K]": 2300.0

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "type": "DISSOLVED_EQUILIBRIUM",
           "condensed phase": "aqueous",
           "algebraic species": "B",
           "solvent": "H2O",
           "reactants": [ { "name": "A" } ],
           "products": [ { "name": "B" } ],
           "equilibrium constant": { "A": 1.14e-2, "C [K]": 2300.0 }
         }

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``"condensed phase"``
     - Yes
     - The phase that the equilibrium takes place in
   * - ``"algebraic species"``
     - Yes
     - The species that the constraint sets. It must be in the condensed phase.
   * - ``solvent``
     - Yes
     - The solvent species in the condensed phase
   * - ``reactants``
     - Yes
     - A list of reactants. Each reactant has a ``name`` and an optional ``coefficient``.
   * - ``products``
     - Yes
     - A list of products. Each product has a ``name`` and an optional ``coefficient``.
   * - ``"equilibrium constant"``
     - Yes
     - See :ref:`v1-aerosol-rate-constants`

Linear Constraint
=================

A weighted sum of species concentrations is constant. Use this constraint for mass
conservation or charge balance:

.. math::

   \sum_i c_i [\mathrm{X}_i] - C = 0

where :math:`c_i` is the coefficient of term :math:`i`, :math:`[\mathrm{X}_i]` is the
concentration of the species in term :math:`i`, and :math:`C` is the constant.

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         type: LINEAR_CONSTRAINT
         "algebraic phase": aqueous
         "algebraic species": B
         "constant [mol m-3]": 1.0e-3
         terms:
           - phase: gas
             name: A
             coefficient: 1.0
           - phase: aqueous
             name: A
             coefficient: 1.0
           - phase: aqueous
             name: B
             coefficient: 1.0

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "type": "LINEAR_CONSTRAINT",
           "algebraic phase": "aqueous",
           "algebraic species": "B",
           "constant [mol m-3]": 1.0e-3,
           "terms": [
             { "phase": "gas", "name": "A", "coefficient": 1.0 },
             { "phase": "aqueous", "name": "A", "coefficient": 1.0 },
             { "phase": "aqueous", "name": "B", "coefficient": 1.0 }
           ]
         }

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``"algebraic phase"``
     - Yes
     - The phase of the algebraic species
   * - ``"algebraic species"``
     - Yes
     - The species that the constraint sets. It must be in the algebraic phase.
   * - ``terms``
     - Yes
     - A list of terms. Each term has a ``phase``, a species ``name``, and a ``coefficient``.
       The species must be in that phase.
   * - ``"constant [mol m-3]"``
     - No
     - The fixed value of :math:`C`. All instances of the constraint use this value. The default is 0.
   * - ``"diagnose from state"``
     - No
     - When ``true``, the solver calculates :math:`C` from the initial state of each
       representation instance.

``"constant [mol m-3]"`` and ``"diagnose from state"`` are mutually exclusive.
If you give both, the parser reports an error.

A fixed constant has the same value in each representation instance. If the instances
start with different totals (for example, two droplets with different amounts), set
``"diagnose from state"`` to ``true``.

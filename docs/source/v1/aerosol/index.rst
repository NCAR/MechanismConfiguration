.. _v1-aerosol:

#######
Aerosol
#######

This section describes the aerosol representations and the processes that 
mechanism configuration can define. These are used to configure the :doc:`MIAM <miam:index>` library, 
which solves the aerosol systems, which enables mixed phase solving of condensed-phase chemistry 
and gas-phase chemistry on top of :doc:`MICM <micm:index>`.

The aerosol section is optional, but if include must contain both keys. 
The two top-level keys are ``"aerosol representations"`` and ``"aerosol processes"``:

- ``"aerosol representations"``: a list of the particle populations and the phases in each population.
  See :ref:`v1-aerosol-representations`.
- ``"aerosol processes"``: a list of the processes and constraints that act on the species in those phases.
  See :ref:`v1-aerosol-processes` and :ref:`v1-aerosol-constraints`.

Both lists use the same ``species`` and ``phases`` as the rest of the mechanism.
Like ``species``, ``phases``, and ``reactions``, each list can be inline or can use the
file-list format (see :doc:`../overview`).

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         version: 1.2.0
         name: aerosol example
         species:
           - name: A
             "molecular weight [kg mol-1]": 0.05
           - name: B
           - name: H2O
             "molecular weight [kg mol-1]": 0.018
         phases:
           - name: gas
             species:
               - name: A
                 "diffusion coefficient [m2 s-1]": 1.5e-5
           - name: aqueous
             species:
               - name: A
               - name: B
               - name: H2O
                 "density [kg m-3]": 1000.0
         reactions: []
         aerosol representations:
           - type: SINGLE_MOMENT_MODE
             name: cloud
             phases: [ aqueous ]
             "geometric mean radius [m]": 1.0e-6
             "geometric standard deviation": 1.4
         aerosol processes:
           - type: HENRYS_LAW_PHASE_TRANSFER
             "gas phase": gas
             "gas-phase species": A
             "condensed phase": aqueous
             "condensed-phase species": A
             solvent: H2O
             "Henry's law constant":
               "HLC_ref [mol m-3 Pa-1]": 1.0e-2
               "C [K]": 3000.0
             "accommodation coefficient": 0.1
           - type: DISSOLVED_REACTION
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
           "version": "1.2.0",
           "name": "aerosol example",
           "species": [
             { "name": "A", "molecular weight [kg mol-1]": 0.05 },
             { "name": "B" },
             { "name": "H2O", "molecular weight [kg mol-1]": 0.018 }
           ],
           "phases": [
             {
               "name": "gas",
               "species": [
                 { "name": "A", "diffusion coefficient [m2 s-1]": 1.5e-5 }
               ]
             },
             {
               "name": "aqueous",
               "species": [
                 { "name": "A" },
                 { "name": "B" },
                 { "name": "H2O", "density [kg m-3]": 1000.0 }
               ]
             }
           ],
           "reactions": [],
           "aerosol representations": [
             {
               "type": "SINGLE_MOMENT_MODE",
               "name": "cloud",
               "phases": [ "aqueous" ],
               "geometric mean radius [m]": 1.0e-6,
               "geometric standard deviation": 1.4
             }
           ],
           "aerosol processes": [
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
             },
             {
               "type": "DISSOLVED_REACTION",
               "condensed phase": "aqueous",
               "solvent": "H2O",
               "reactants": [ { "name": "A" } ],
               "products": [ { "name": "B" } ],
               "rate constant": { "type": "ARRHENIUS", "A": 1.0e3, "C": 100.0 }
             }
           ]
         }

Properties from species and phases
==================================

Some aerosol processes need physical properties of a species. You do not put these
properties on the process. The parser gets them from the ``species`` and ``phases``
sections of the mechanism. If a property is missing, the parser reports an error.

.. list-table::
   :header-rows: 1
   :widths: 30 30 40

   * - Property
     - Where you set it
     - Used by
   * - ``"diffusion coefficient [m2 s-1]"``
     - The gas-phase species, in the gas phase
     - ``HENRYS_LAW_PHASE_TRANSFER``
   * - ``"density [kg m-3]"``
     - The solvent, in the condensed phase
     - ``HENRYS_LAW_EQUILIBRIUM``
   * - ``"molecular weight [kg mol-1]"``
     - The solvent, in the ``species`` list
     - ``HENRYS_LAW_EQUILIBRIUM``

.. toctree::
   :maxdepth: 1
   :caption: Contents:

   representations
   rate_constants
   processes
   constraints

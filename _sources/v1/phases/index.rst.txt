.. _v1-phases:

######
Phases
######

Phases are collections of species that are considered to be well-mixed and
occupy a volume of arbitrary shape:

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         phases:
           - name: my phase
             species:
               - name: foo
                 "diffusion coefficient [m2 s-1]": 1.0
               - name: bar
                 "__custom property": 0.5
                 "__another custom property": "value"
             "__my custom phase property": "custom value"

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "phases": [
             {
               "name": "my phase",
               "species": [
                 {
                   "name": "foo",
                   "diffusion coefficient [m2 s-1]": 1.0
                 },
                 {
                   "name": "bar",
                   "__custom property": 0.5,
                   "__another custom property": "value"
                 }
               ],
               "__my custom phase property": "custom value"
             }
           ]
         }


The ``name`` of the phase is required and must be unique.
As the current version of Mechanism Configuration only supports gas phases,
a single phase named ``gas`` should be present in all configurations, and
should contain all gas-phase species.

The species array is required and must contain one or more elements.
The ``name`` property is required for each element in the species
array and must match the name of an element of the top-level species
array (see :ref:`v1-chemical-species`). The same species can appear in
multiple phases.

Each element in the species array can include optional properties:

- ``"diffusion coefficient [m2 s-1]"``: The diffusion coefficient of the species in the phase.

These optional properties are unique to the species and phase combination. They are
only required when a reaction requires that a particular property be defined (e.g.,
evaporation/condensation processes).

The phase and elements of the species array can be decorated with custom properties
that are prefixed with double underscores (e.g., ``__custom property``). These custom
properties can be used to store additional information about the phase or species that
is not covered by the standard properties.
.. _chemical-species:

################
Chemical Species
################

An object containing information about a chemical species has the following format:

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         camp-data:
           - name: my species name
             type: CHEM_SPEC
           - name: my other species name
             type: CHEM_SPEC
             "molecular weight [kg mol-1]": 0.025
             "diffusion coefficient [m2 s-1]": 2.3e2


   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "camp-data": [
             {
               "name": "my species name",
               "type": "CHEM_SPEC"
             },
             {
               "name": "my other species name",
               "type": "CHEM_SPEC",
               "molecular weight [kg mol-1]": 0.025,
               "diffusion coefficient [m2 s-1]": 2.3e2
             }
           ]
         }


The key-value pair ``name`` is required and must contain the unique name used for this
species.
The key-value pair ``type`` is also required, and must be ``CHEM_SPEC``.
Optional key-value pairs are used to define physical and chemical properties.
These include:

- ``"molecular weight [kg mol-1]"``
- ``"diffusion coefficient [m2 s-1]"``

These optional properties are only required when a species participates in a
reaction that requires that a particular property be defined (e.g., surface reactions).

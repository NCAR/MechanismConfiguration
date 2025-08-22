.. _v1-chemical-species:

################
Chemical Species
################

An object containing information about a chemical species has the following format:

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         species:
           - name: foo
           - name: bar
             "tracer type": THIRD_BODY
           - name: baz
             "molecular weight [kg mol-1]": 0.025
             "diffusion coefficient [m2 s-1]": 2.3e2
             "__my custom property": "custom value"
             "__my other custom property": 6.73


   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
            "species": [
              {
                "name": "foo"
              },
              {
                "name": "bar",
                "tracer type": "THIRD_BODY"
              },
              {
                "name": "baz",
                "molecular weight [kg mol-1]": 0.025,
                "__my custom property": "custom value",
                "__my other custom property": 6.73
              }
            ]
         }


The key-value pair ``name`` is required and must contain the unique name used for this
species. All other key-value pairs are optional.

When included, the key-value pair ``tracer type``, must be one of the following:

- ``THIRD_BODY``

Optional key-value pairs are used to define physical and chemical properties.
These include:

- ``"molecular weight [kg mol-1]"``: The molecular weight of the species.

These optional properties are only required when a species participates in a
reaction that requires that a particular property be defined (e.g., surface reactions).

Species configuration data can be decorated with custom properties that are prefixed with
double underscores (e.g., ``__my custom property``). These custom properties can be used to
store additional information about the species that is not covered by the standard properties.

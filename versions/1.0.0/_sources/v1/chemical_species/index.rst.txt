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
             "is third body": true
           - name: baz
             "molecular weight [kg mol-1]": 0.025
             "constant concentration [mol m-3]": 1.0e19
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
                "is third body": true
              },
              {
                "name": "baz",
                "molecular weight [kg mol-1]": 0.025,
                "constant concentration [mol m-3]": 1.0e19,
                "__my custom property": "custom value",
                "__my other custom property": 6.73
              }
            ]
         }


The key-value pair ``name`` is required and must contain the unique name used for this
species. All other key-value pairs are optional.

Optional key-value pairs are used to define physical and chemical properties.
These include:

- ``"molecular weight [kg mol-1]"``: The molecular weight of the species.
- ``"constant concentration [mol m-3]"``: The constant concentration of the species.
- ``"constant mixing ratio [mol mol-1]"``: The constant volume mixing ratio of the species.
- ``"is third body"``: When present and true, the species concentration calculated as the total air density.

These optional properties are only required when a species participates in a
reaction that requires that a particular property be defined (e.g., surface reactions),
or to specify how the species should be treated by the solver (e.g., as a constant species).
Only one of ``"constant concentration [mol m-3]"``,
``"constant mixing ratio [mol mol-1]"``, or ``"is third body"`` should be provided for a given species.

Species configuration data can be decorated with custom properties that are prefixed with
double underscores (e.g., ``__my custom property``). These custom properties can be used to
store additional information about the species that is not covered by the standard properties.

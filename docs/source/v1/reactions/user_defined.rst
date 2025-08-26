User Defined
============

User-defined reactions are intended to handle cases where a custom algorithm is needed to calculate
the rate constant for a reaction:

.. math::

   \ce{X_1 ( + X_2 \dots ) -> Y_1 ( + Y_2 \dots )}

where :math:`\ce{X_n}` are the reactants, and :math:`\ce{Y_n}` are the products of the reaction.

User defined rate constants are constant over the solver time step.
A conforming implementation of version 1.0 of the configuration will provide ways
to set the user defined rate constant using the name of the reaction.

Input data for user defined reactions have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: USER_DEFINED
            name: foo-user-defined
            gas phase: gas
            scaling factor: 1.2
            reactants:
              - species name: foo
                coefficient: 1.0
            products:
              - species name: bar
                coefficient: 0.5
              - species name: baz
                coefficient: 0.3


    .. tab-item:: JSON

        .. code-block:: json

            {
                "type": "USER_DEFINED",
                "name": "foo-user-defined",
                "gas phase": "gas",
                "scaling factor": 1.2,
                "reactants": [
                    {
                        "species name": "foo",
                        "coefficient": 1.0
                    }
                ],
                "products": [
                    {
                        "species name": "bar",
                        "coefficient": 0.5
                    },
                    {
                        "species name": "baz",
                        "coefficient": 0.3
                    }
                ]
            }

The key-value pairs ``reactants`` and ``products`` are required.
Any number of reactants and products may be present. Reactants and products without a specified ``coefficient`` are
assumed to have a ``coefficient`` of 1.0.

The ``gas phase`` key is required and must be set to the name of the phase the
reaction takes place in. Each reactant and product must be present in the specified phase.

The ``scaling factor`` is optional and can be used to set a constant scaling factor for
the rate constant. When the ``scaling factor`` is not provided, it is assumed to be 1.0.

Rate constants are in units of :math:`\mathrm{(m^{3}\ mol^{-1})^{(n-1)}\ s^{-1}}` where :math:`n` is the total number of reactants.

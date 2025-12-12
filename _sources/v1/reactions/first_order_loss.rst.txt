First-Order Loss
================

First-Order Loss reactions take the form:

.. math::

   \ce{X} \rightarrow

where :math:`\ce{X}` is the species being lost.

First-Order loss rate constants are constant over the solver time step.
A conforming implementation of version 1.0 of the configuration will
provide ways to set the loss rate using the name of the reaction.

Input data for first-order loss reactions have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: FIRST_ORDER_LOSS
            name: foo-loss
            scaling factor: 1.2
            gas phase: foo-gas
            reactants:
              - species name: foo


    .. tab-item:: JSON

        .. code-block:: json

            {
                "type": "FIRST_ORDER_LOSS",
                "name": "foo-loss",
                "scaling factor": 1.2,
                "gas phase": "foo-gas",
                "reactants": [
                    {
                        "species name": "foo"
                    }
                ]
            }

The ``reactants`` key is required and must be set to a list of species being lost.
The ``coefficient`` key is optional and, if not provided, is assumed to be 1.0.

The ``gas phase`` key is required and must be set to the name of the phase the
reaction takes place in. Each reactant must be present in the specified phase.

The ``scaling factor`` is optional and can be used to set a constant scaling factor for the rate.
When a ``scaling factor`` is not provided, it is assumed to be 1.0.

Rate constants are in :math:`\mathrm{s^{-1}}`.

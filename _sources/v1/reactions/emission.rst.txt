Emission
========

Emissions take the form:

.. math::

   \rightarrow \ce{X}

where :math:`\ce{X}` is the species being emitted.

Emission rates are constant over the solver time step.
A conforming implementation of version 1.0 of the configuration will
provide ways to set the emission rate using the name of the reaction.

Input data for emissions have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: EMISSION
            name: foo-emit
            gas phase: gas
            scaling factor: 1.2
            products:
              - species name: foo
                coefficient: 2.0


    .. tab-item:: JSON

        .. code-block:: json

            {
                "type": "EMISSION",
                "name": "foo-emit",
                "gas phase": "gas",
                "scaling factor": 1.2,
                "products": [
                    {"species name": "foo", "coefficient": 2.0}
                ]
            }

The ``products`` key is required and must be set to a list of species being emitted.
The ``coefficient`` key is optional and, if not provided, is assumed to be 1.0.

The ``gas phase`` key is required and must be set to the name of the phase the
reaction takes place in. Each product must be present in the specified phase.

The ``scaling factor`` is optional and can be used to set a constant scaling factor for the rate.
When a ``scaling factor`` is not provided, it is assumed to be 1.0.

Rates are in :math:`\mbox{mol}\,\mathrm{m}^{-3}\,\mathrm{s^{-1}}`.

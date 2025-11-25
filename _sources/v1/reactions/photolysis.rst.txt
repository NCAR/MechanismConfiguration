Photolysis
==========

Photolysis reactions take the form:

.. math::

   \ce{X + h $\nu$ -> Y_1 ( + Y_2 \dots )}

where :math:`\ce{X}` is the species being photolyzed, and :math:`\ce{Y_n}` are the photolysis products.

Photolysis rate constants (including the :math:`\ce{h $\nu$}` term) are constant
over the solver time step.
A conforming implementation of version 1.0 of the configuration will provide ways
to set the photolysis rate constant using the name of the reaction.

Input data for photolysis reactions have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: PHOTOLYSIS
            name: foo-photo
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
                "type": "PHOTOLYSIS",
                "name": "foo-photo",
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
There should be exactly one reactant, which is the species being photolyzed.
Any number of products may be present. Products without a specified ``coefficient`` are
assumed to have a ``coefficient`` of 1.0.

The ``gas phase`` key is required and must be set to the name of the phase the
reaction takes place in. Each reactant and product must be present in the specified phase.

The ``scaling factor`` is optional and can be used to set a constant scaling factor for
the rate constant. 
When the ``scaling factor`` is not provided, it is assumed to be 1.0.

Rate constants are in units of :math:`\mathrm{s}^{-1}`.

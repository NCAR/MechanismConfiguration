First-Order Loss
================

First-Order Loss reactions take the form:

.. math::

   \ce{X} \rightarrow

where :math:`\ce{X}` is the species being lost.

First-Order loss rate constants are constant.
A conforming implementation of version |version| of the configuration will provide ways to dynamically update wet deposition rates if nonconstant rates are needed.

Input data for first-order loss reactions have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: FIRST_ORDER_LOSS
            species: species name
            scaling factor: 1.2


    .. tab-item:: JSON

        .. code-block:: json

            {
                "type": "FIRST_ORDER_LOSS",
                "species": "species name",
                "scaling factor": 1.2
            }

The key-value pair ``species`` is required, and its value must be the name of the species being removed by the reaction. 
The ``scaling factor`` is optional and can be used to set a constant scaling factor for the rate constant. 
When a ``scaling factor`` is not provided, it is assumed to be 1.0. 
All other data is optional and will be available to external modules during initialization. 
Rate constants are in units of :math:`(\mathrm{s}^{-1})`.

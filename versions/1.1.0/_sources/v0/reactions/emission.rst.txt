Emission
========

Emission reactions take the form:

.. math::

   \rightarrow \ce{X}

where :math:`\ce{X}` is the species being emitted.

Emission rates are constant.
A conforming implementation of version |version| of the configuration will provide ways to dynamically update wet deposition rates if nonconstant rates are needed.

Input data for emission reactions have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: EMISSION
            species: species name
            scaling factor: 1.2


    .. tab-item:: JSON

        .. code-block:: json

            {
                "type": "EMISSION",
                "species": "species name",
                "scaling factor": 1.2,
            }

The key-value pair ``species`` is required, and its value must be the name of the species being emitted. 
The ``scaling factor`` is optional and can be used to set a constant scaling factor for the rate. 
When a ``scaling factor`` is not provided, it is assumed to be 1.0. 
Rates are in :math:`\mbox{molecule}\,\mathrm{cm}^{-3}\,\mathrm{s^{-1}}`.

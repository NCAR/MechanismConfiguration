Photolysis
==========

Photolysis reactions take the form:

.. math::

   \ce{X + h $\nu$ -> Y_1 ( + Y_2 \dots )}

where :math:`\ce{X}` is the species being photolyzed, and :math:`\ce{Y_n}` are the photolysis products.

Photolysis rate constants (including the :math:`\ce{h $\nu$}` term) can be constant. 
A conforming implementation of version |version| of the configuration will provide ways to dynamically update photolysis if nonconstant rates are needed.

Input data for photolysis reactions have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: PHOTOLYSIS
            reactants:
                spec1:
            products:
                spec2:
                spec3:
                    yield: 0.65
            scaling factor: 1.2


    .. tab-item:: JSON

        .. code-block:: json

            {
                "type": "PHOTOLYSIS",
                "reactants": {
                    "spec1": {}
                },
                "products": {
                    "spec2": {},
                    "spec3": {
                        "yield": 0.65
                    }
                },
                "scaling factor": 1.2
            }

The key-value pairs ``reactants`` and ``products`` are required.
There must be exactly one key-value pair in the ``reactants`` object whose name is the species being photolyzed and whose value is an empty JSON object.
Any number of products may be present. Products without a specified ``yield`` are assumed to have a ``yield`` of 1.0. 
The ``scaling factor`` is optional and can be used to set a constant scaling factor for the rate constant. 
When the ``scaling factor`` is not provided, it is assumed to be 1.0. All other data is optional and will be available to external photolysis modules during initialization.
Rate constants are in units of :math:`\mathrm{s}^{-1}`.

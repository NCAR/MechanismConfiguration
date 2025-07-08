Condensed-Phase Photolysis Reaction
===================================

Condensed-Phase Photolysis reactions take the form:

.. math::

   \ce{X + h \nu -> Y_1 (+ Y_2 \dots)}

where :math:`\ce{X}` is the species being photolyzed, and :math:`\ce{Y_n}` are the photolysis products.

The reaction rate can be scaled by providing the "scaling factor" keyword in the JSON configuration.

Input data for condensed-phase Photolysis reactions have the following format:

.. tabs::

    .. tab:: YAML

        .. code-block:: yaml

            type: CONDENSED_PHASE_PHOTOLYSIS
            rate: 123.45
            units: M
            aerosol phase: my aqueous phase
            aerosol-phase water: H2O_aq
            reactants:
              spec1: {}
              ...
            products:
              spec3: {}
              spec4:
                yield: 0.65
              ...
            scaling factor: 11.20

    .. tab:: JSON

        .. code-block:: json

            {
                "type": "CONDENSED_PHASE_PHOTOLYSIS",
                "rate": 123.45,
                "units": "M",
                "aerosol phase": "my aqueous phase",
                "aerosol-phase water": "H2O_aq",
                "reactants": {
                    "spec1": {},
                },
                "products": {
                    "spec3": {},
                    "spec4": { "yield": 0.65 },
                },
                "scaling factor": 11.20
            }

The key-value pairs ``reactants`` and ``products`` are required. Reactants without a ``qty`` value are assumed to appear once in the reaction equation. Products without a specified ``yield`` are assumed to have a yield of 1.0.

Units for the reactants and products must be specified using the key ``units`` and can be either ``M`` or ``mol m-3``. If units of ``M`` are specified, a key-value pair ``aerosol-phase water`` must also be included, whose value is a string specifying the name for water in the aerosol phase.

The unit for time is assumed to be s, but inclusion of the optional key-value pair ``time unit = MIN`` can be used to indicate a rate with minutes as the time unit.

The key-value pair ``aerosol phase`` is required and must specify the name of the aerosol phase in which the reaction occurs.

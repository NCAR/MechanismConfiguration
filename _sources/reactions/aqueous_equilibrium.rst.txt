Aqueous-Phase Equilibrium
=========================

Aqueous equilibrium reactions are calculated as forward and reverse reactions.
The reverse rate constant must be provided as part of the input data, and the forward rate constant is calculated using the provided 
reverse rate constant and an equilibrium constant of the following format:

.. math::

   A e^{C \left(\frac{1}{T} - \frac{1}{298}\right)}

where:

- :math:`A` is the pre-exponential factor (:math:`\mathrm{s}^{-1}`),
- :math:`C` is a constant,
- :math:`T` is the temperature :math:`(\mathrm{K})`.

Input data for aqueous equilibrium equations should take the form:

.. tabs::

    .. tab:: YAML

        .. code-block:: yaml

            type: AQUEOUS_EQUILIBRIUM
            A: 123.45
            C: 123.45
            k_reverse: 123.45
            phase: "my aqueous phase"
            time unit: MIN
            aqueous-phase water: "H2O_aq"
            ion pair: "spec3-spec4"
            reactants:
              spec1: {}
              spec2:
                qty: 2
            # ... (other reactants)
            products:
              spec3: {}
              spec4:
                qty: 0.65
            # ... (other products)

    .. tab:: JSON

        .. code-block:: json

            {
                "type" : "AQUEOUS_EQUILIBRIUM",
                "A" : 123.45,
                "C" : 123.45,
                "k_reverse" : 123.45,
                "phase" : "my aqueous phase",
                "time unit" : "MIN",
                "aqueous-phase water" : "H2O_aq",
                "ion pair" : "spec3-spec4",
                "reactants" : {
                    "spec1" : {},
                    "spec2" : { "qty" : 2 },
                },
                "products" : {
                    "spec3" : {},
                    "spec4" : { "qty" : 0.65 },
                }
            }

The key-value pairs ``reactants`` and ``products`` are required. Reactants and products without a ``qty`` value are assumed to appear once
in the reaction equation. Reactant and product species must be present in the specified phase and include a ``molecular weight [kg mol-1]`` 
parameter in :math:`\mathrm{kg}\ \mathrm{mole}^{-1}`. The parameter ``aqueous-phase water`` is required and must be the name of the aerosol-phase species that is used for water.
The parameter ``ion pair`` is optional. When it is included, its value must be the name of an ion pair that is present in the specified aerosol phase.
Its mean binary activity coefficient will be applied to the reverse reaction.

When ``A`` is not included, it is assumed to be 1.0; when ``C`` is not included, it is assumed to be 0.0. The reverse reaction rate constant ``k_reverse`` is required.

The unit for time is assumed to be s, but inclusion of the optional key-value pair ``time unit = MIN`` can be used to indicate a rate with minutes as the time unit.

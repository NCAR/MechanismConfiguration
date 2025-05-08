Special CMAQ Reaction for OH+HNO3
=================================

For the reaction:

.. math::

   \ce{
   OH + HNO3 ->[k] NO3 + H2O
   }

CMAQ rate constants are calculated as:

.. math::

   k = k_0 + \left( \frac{k_3 [\mathrm{M}]}{1 + \frac{k_3 [\mathrm{M}]}{k_2}} \right)

where :math:`k_0`, :math:`k_2`, and :math:`k_3` are `Arrhenius` rate constants with :math:`D = 300` and :math:`E = 0`, and :math:`[\mathrm{M}]` is the concentration of air :cite:`Gipson`.

Input data for CMAQ :math:`\ce{OH + HNO3}` reactions have the following format:

.. tabs::

    .. tab:: YAML

        .. code-block:: yaml

            type: CMAQ_OH_HNO3
            k0_A: 5.6E-12
            k0_B: -1.8
            k0_C: 180.0
            k2_A: 3.4E-12
            k2_B: -1.6
            k2_C: 104.1
            k3_A: 3.2E-11
            k3_B: -1.5
            k3_C: 92.0
            time unit: MIN
            reactants:
              spec1: {}
              spec2:
                qty: 2
            # ... (other reactants)
            products:
              spec3: {}
              spec4:
                yield: 0.65
            # ... (other products)

    .. tab:: JSON

        .. code-block:: json

            {
                "type": "CMAQ_OH_HNO3",
                "k0_A": 5.6E-12,
                "k0_B": -1.8,
                "k0_C": 180.0,
                "k2_A": 3.4E-12,
                "k2_B": -1.6,
                "k2_C": 104.1,
                "k3_A": 3.2E-11,
                "k3_B": -1.5,
                "k3_C": 92.0,
                "time unit": "MIN",
                "reactants": {
                    "spec1": {},
                    "spec2": { "qty": 2 },
                },
                "products": {
                    "spec3": {},
                    "spec4": { "yield": 0.65 },
                }
            }

The key-value pairs ``reactants`` and ``products`` are required. Reactants without a ``qty`` value are assumed to appear once in the reaction equation. Products without a specified ``yield`` are assumed to have a yield of 1.0.

The three sets of parameters beginning with ``k0_``, ``k2_``, and ``k3_`` are the `Arrhenius` parameters for the :math:`k_0`, :math:`k_2`, and :math:`k_3` rate constants, respectively. When not present, ``_A`` parameters are assumed to be 1.0, ``_B`` to be 0.0, and ``_C`` to be 0.0.

The unit for time is assumed to be s, but inclusion of the optional key-value pair ``time unit = MIN`` can be used to indicate a rate with minutes as the time unit.

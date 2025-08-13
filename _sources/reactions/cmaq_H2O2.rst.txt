Special CMAQ Reaction for H2O2
==============================

For the reactions:

.. math::

   \ce{
   HO2 + HO2 ->[k] H2O2
   }

.. math::

   \ce{
   HO2 + HO2 + H2O ->[k] H2O2
   }

CMAQ rate constants are calculated as:

.. math::

   k = k_1 + k_2 [\mathrm{M}]

where :math:`k_1` and :math:`k_2` are `Arrhenius` rate constants with :math:`D = 300` and :math:`E = 0`, and :math:`[\mathrm{M}]` is the concentration of air :cite:`Gipson`.

Input data for CMAQ :math:`\ce{H2O2}` reactions have the following format:

.. tabs::

    .. tab:: YAML

        .. code-block:: yaml

            type: CMAQ_H2O2
            k1_A: 5.6E-12
            k1_B: -1.8
            k1_C: 180.0
            k2_A: 3.4E-12
            k2_B: -1.6
            k2_C: 104.1
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
                "type": "CMAQ_H2O2",
                "k1_A": 5.6E-12,
                "k1_B": -1.8,
                "k1_C": 180.0,
                "k2_A": 3.4E-12,
                "k2_B": -1.6,
                "k2_C": 104.1,
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

The two sets of parameters beginning with ``k1_`` and ``k2_`` are the `Arrhenius` parameters for the :math:`k_1` and :math:`k_2` rate constants, respectively. When not present, ``_A`` parameters are assumed to be 1.0, ``_B`` to be 0.0, and ``_C`` to be 0.0.

The unit for time is assumed to be s, but inclusion of the optional key-value pair ``time unit = MIN`` can be used to indicate a rate with minutes as the time unit.

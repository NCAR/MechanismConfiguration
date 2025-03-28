Troe Reaction
=============

Troe (fall-off) reaction rate constant equations take the form:

.. math::

   \frac{k_0[\mathrm{M}]}{1 + \frac{k_0[\mathrm{M}]}{k_{\infty}}} F_C^{\left(1 + \frac{1}{N} [\log_{10}(\frac{k_0[\mathrm{M}]}{k_{\infty}})]^2\right)^{-1}}

where:

- :math:`k_0` is the low-pressure limiting rate constant,
- :math:`k_{\infty}` is the high-pressure limiting rate constant,
- :math:`[\mathrm{M}]` is the density of air,
- :math:`F_C` and :math:`N` are parameters that determine the shape of the fall-off curve,
  and are typically 0.6 and 1.0, respectively :cite:`Finlayson-Pitts2000` :cite:`Gipson`.

:math:`k_0` and :math:`k_{\infty}` are calculated as `Arrhenius` rate constants with :math:`D=300` and :math:`E=0`.

Input data for Troe reactions have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: TROE
            k0_A: 5.6e-12
            k0_B: -1.8
            k0_C: 180.0
            kinf_A: 3.4e-12
            kinf_B: -1.6
            kinf_C: 104.1
            Fc: 0.7
            N: 0.9
            time unit: MIN
            reactants:
                spec1: {}
                spec2:
                    qty: 2
            products:
                spec3: {}
                spec4:
                    yield: 0.65


    .. tab-item:: JSON

        .. code-block:: json

            {
                "type": "TROE",
                "k0_A": 5.6e-12,
                "k0_B": -1.8,
                "k0_C": 180.0,
                "kinf_A": 3.4e-12,
                "kinf_B": -1.6,
                "kinf_C": 104.1,
                "Fc": 0.7,
                "N": 0.9,
                "time unit": "MIN",
                "reactants": {
                    "spec1": {},
                    "spec2": {
                        "qty": 2
                    }
                },
                "products": {
                    "spec3": {},
                    "spec4": {
                        "yield": 0.65
                    }
                }
            }

The key-value pairs ``reactants`` and ``products`` are required. Reactants without a ``qty`` value
are assumed to appear once in the reaction equation. Products without a specified ``yield`` are
assumed to have a yield of 1.0.

The two sets of parameters beginning with ``k0_`` and ``kinf_`` are the `Arrhenius` parameters for the
:math:`k_0` and :math:`k_{\infty}` rate constants, respectively. When not present, ``_A`` parameters are
assumed to be 1.0, ``_B`` to be 0.0, ``_C`` to be 0.0, ``Fc`` to be 0.6, and ``N`` to be 1.0.

The unit for time is assumed to be s, but inclusion of the optional key-value pair ``time unit`` = ``MIN``
can be used to indicate a rate with minutes as the time unit.


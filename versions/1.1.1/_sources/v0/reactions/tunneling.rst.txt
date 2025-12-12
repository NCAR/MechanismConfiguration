Wennberg Tunneling Reaction
===========================

Wennberg tunneling reaction rate constant equations are calculated as follows:

.. math::

   Ae^{-\left(\frac{B}{T}\right)}e^{\left(\frac{C}{T^3}\right)}

where ``A`` is the pre-exponential factor
:math:`[(\mathrm{molecule}\,\mathrm{cm}^{-3})^{-(n-1)} s^{-1}]`,
and ``B`` and ``C`` are parameters that capture the temperature
dependence as described in Wennberg et al. (2018) :cite:`Wennberg2018`.

Input data for Wennberg tunneling equations has the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: WENNBERG_TUNNELING
            A: 123.45
            B: 1200.0
            C: 1.0e8
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
                "type" : "WENNBERG_TUNNELING",
                "A" : 123.45,
                "B"  : 1200.0,
                "C"  : 1.0e8,
                "time unit" : "MIN",
                "reactants" : {
                    "spec1" : {},
                    "spec2" : { "qty" : 2 }
                },
                "products" : {
                    "spec3" : {},
                    "spec4" : { "yield" : 0.65 }
                }
            }

The key-value pairs ``reactants``, and ``products`` are required. Reactants
without a ``qty`` value are assumed to appear once in the reaction equation.
Products without a specified ``yield`` are assumed to have a yield of
1.0.

When ``A`` is not included, it is assumed to be 1.0, when ``B`` is not
included, it is assumed to be :math:`0.0\ \mathrm{K}`, and when ``C`` is not included, it is
assumed to be :math:`0.0\ \mathrm{K}^3`.
The unit for time is assumed to be :math:`\mathrm{s}`, but inclusion of the optional
key-value pair ``time unit`` = ``MIN`` can be used to indicate a rate
with min as the time unit.

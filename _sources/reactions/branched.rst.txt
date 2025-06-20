Branched(Wennberg NO + RO2 Reaction)
====================================

Wennberg NO + RO2 reactions are branched reactions with one branch forming
alkoxy radicals plus NO2 and the other forming organic nitrates :cite:`Wennberg2018`.
The rate constants for each branch are based on an Arrhenius rate constant and
a temperature- and structure-dependent branching ratio calculated as:

.. math::

   k_{\text{nitrate}} = (X e^{-Y/T}) \left(\frac{A(T, [\text{M}], n)}{A(T, [\text{M}], n) + Z}\right) 

.. math::

   k_{\text{alkoxy}} = (X e^{-Y/T})\left(\frac{Z}{Z + A(T, [\text{M}], n)}\right) 

.. math::

   A(T, [\text{M}], n) = \frac{2 \times 10^{-22} e^n [\text{M}]}{1 + \frac{2 \times 10^{-22} e^n [\text{M}]}{0.43(T/298)^{-8}}} 0.41^{(1+[\log( \frac{2 \times 10^{-22} e^n [\text{M}]}{0.43(T/298)^{-8}})]^2)^{-1}}

where :math:`T` is temperature :math:`(\mathrm{K})`, :math:`[\mathrm{M}]` is the number density of air :math:`(\mathrm{molecules}\ \mathrm{cm}^{-3})`,
:math:`X` and :math:`Y` are Arrhenius parameters for the overall reaction, :math:`n` is the number of
heavy atoms in the RO2 reacting species (excluding the peroxy moiety), and :math:`Z`
is defined as a function of two parameters :math:`(\alpha_0, n)`:

.. math::

   Z(\alpha_0, n) = A(T = 293 K, [M] = 2.45 \times 10^{19} \frac{\text{molec}}{\text{cm}^3}, n) \frac{(1-α_0)}{α_0}

More details can be found in Wennberg et al. (2018) :cite:`Wennberg2018`.

Input data for Wennberg NO + RO2 equations has the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: WENNBERG_NO_RO2
            X: 123.45
            Y: 1200.0
            a0: 1.0e8
            n: 6
            time unit: MIN
            reactants:
                spec1:
                spec2:
                  qty: 2
            alkoxy products:
                spec3:
                spec4:
                  yield: 0.65
            nitrate products:
                spec5:
                spec6:
                  yield: 0.32


    .. tab-item:: JSON

        .. code-block:: json

            {
                "type" : "WENNBERG_NO_RO2",
                "X" : 123.45,
                "Y"  : 1200.0,
                "a0"  : 1.0e8,
                "n" : 6,
                "time unit" : "MIN",
                "reactants" : {
                  "spec1" : {},
                  "spec2" : { "qty" : 2 },
                },
                "alkoxy products" : {
                  "spec3" : {},
                  "spec4" : { "yield" : 0.65 },
                },
                "nitrate products" : {
                  "spec5" : {},
                  "spec6" : { "yield" : 0.32 },
                }
            }


The key-value pairs ``reactants``, ``alkoxy products``, and ``nitrate products`` are required.
Reactants without a ``qty`` value are assumed to appear once in the reaction equation.
Products without a specified ``yield`` are assumed to have a yield of 1.0.

When ``X`` is not included, it is assumed to be 1.0, when ``Y`` is not
included, it is assumed to be 0.0 K, when ``a0`` is not included, it is
assumed to be 1.0, and when ``n`` is not included, it is assumed to be 0.
The unit for time is assumed to be s, but inclusion of the optional
key-value pair ``time unit`` = ``MIN`` can be used to indicate a rate
with min as the time unit.

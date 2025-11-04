Branched NO + RO2 Reaction
==========================

Branched reactions are typically used for NO + RO2 reactions with one branch forming
alkoxy radicals plus NO2 and the other forming organic nitrates :cite:`Wennberg2018`.
The rate constants for each branch are based on an Arrhenius rate constant and
a temperature- and structure-dependent branching ratio calculated as:

.. math::

   k_{\text{nitrate}} = (X e^{-Y/T}) \left(\frac{A(T, [\text{M}], n)}{A(T, [\text{M}], n) + Z}\right) 

.. math::

   k_{\text{alkoxy}} = (X e^{-Y/T})\left(\frac{Z}{Z + A(T, [\text{M}], n)}\right) 

.. math::

   A(T, [\text{M}], n) = \frac{2 \times 10^{-22} e^n [\text{M}]}{1 + \frac{2 \times 10^{-22} e^n [\text{M}]}{0.43(T/298)^{-8}}} 0.41^{(1+[\log( \frac{2 \times 10^{-22} e^n [\text{M}]}{0.43(T/298)^{-8}})]^2)^{-1}}

where :math:`T` is temperature :math:`(\mathrm{K})`, :math:`[\mathrm{M}]` is the number density of air :math:`(\mathrm{mol}\ \mathrm{m}^{-3})`,
:math:`X` and :math:`Y` are Arrhenius parameters for the overall reaction, :math:`n` is the number of
heavy atoms in the RO2 reacting species (excluding the peroxy moiety), and :math:`Z`
is defined as a function of two parameters :math:`(\alpha_0, n)`:

.. math::

   Z(\alpha_0, n) = A(T = 293 K, [M] = 40.6832 \frac{\text{mol}}{\text{m}^3}, n) \frac{(1-α_0)}{α_0}

More details can be found in Wennberg et al. (2018) :cite:`Wennberg2018`.

Input data for Branched reactions have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: BRANCHED_NO_RO2
            name: foo-branched
            X: 123.45
            Y: 1200.0
            a0: 1.0e8
            n: 6
            gas phase: gas
            reactants:
              - species name: foo
                coefficient: 2.0
              - species name: bar
            alkoxy products:
              - species name: baz
              - species name: qux
                coefficient: 0.65
            nitrate products:
              - species name: corge
              - species name: grault
                coefficient: 0.32


    .. tab-item:: JSON

        .. code-block:: json

            {
                "type": "BRANCHED_NO_RO2",
                "name": "foo-branched",
                "X": 123.45,
                "Y": 1200.0,
                "a0": 1.0e8,
                "n": 6,
                "gas phase": "gas",
                "reactants": [
                    {"species name": "foo", "coefficient": 2.0},
                    {"species name": "bar"}
                ],
                "alkoxy products": [
                    {"species name": "baz"},
                    {"species name": "qux", "coefficient": 0.65}
                ],
                "nitrate products": [
                    {"species name": "corge"},
                    {"species name": "grault", "coefficient": 0.32}
                ]
            }


The key-value pairs ``reactants``, ``alkoxy products``, and ``nitrate products`` are required.
If ``coefficient`` is not specified for a reactant or product, it is assumed to be 1.0.

The ``gas phase`` key is required and must be set to the name of the phase the
reaction takes place in. Each reactant and product must be present in the specified phase.

When ``X`` is not included, it is assumed to be 1.0, when ``Y`` is not
included, it is assumed to be 0.0 K, when ``a0`` is not included, it is
assumed to be 1.0, and when ``n`` is not included, it is assumed to be 0.

Rate constants are in units of :math:`\mathrm{(m^{3}\ mol^{-1})^{(n-1)}\ s^{-1}}` where :math:`n` is the total number of reactants.

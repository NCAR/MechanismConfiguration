Tunneling Reaction
==================

Tunneling reaction rate constant equations are calculated as follows:

.. math::

   Ae^{-\left(\frac{B}{T}\right)}e^{\left(\frac{C}{T^3}\right)}

where ``A`` is the pre-exponential factor
:math:`[(\mathrm{mol}\,\mathrm{m}^{-3})^{-(n-1)} s^{-1}]`,
and ``B`` and ``C`` are parameters that capture the temperature
dependence as described in Wennberg et al. (2018) :cite:`Wennberg2018`.

Input data for Tunneling reactions have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: TUNNELING
            name: foo-tunneling
            A: 123.45
            B: 1200.0
            C: 1.0e8
            gas phase: gas
            reactants:
              - species name: foo
              - species name: bar
                coefficient: 2
            products:
              - species name: baz
              - species name: qux
                coefficient: 0.65

    .. tab-item:: JSON

        .. code-block:: json

            {
                "type": "TUNNELING",
                "A": 123.45,
                "B": 1200.0,
                "C": 1.0e8,
                "gas phase": "gas",
                "reactants": [
                    {"species name": "foo"},
                    {"species name": "bar", "coefficient": 2}
                ],
                "products": [
                    {"species name": "baz"},
                    {"species name": "qux", "coefficient": 0.65}
                ]
            }

The key-value pairs ``reactants``, and ``products`` are required. When a ``coefficient`` is not
specified for a reactant or product, it is assumed to be 1.0.

The ``gas phase`` key is required and must be set to the name of the phase the reaction
takes place in. The reactants and products must be present in the specified phase.

When ``A`` is not included, it is assumed to be 1.0, when ``B`` is not
included, it is assumed to be :math:`0.0\ \mathrm{K}`, and when ``C`` is not included, it is
assumed to be :math:`0.0\ \mathrm{K}^3`.

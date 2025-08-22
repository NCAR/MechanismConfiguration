Arrhenius Reaction Rates
========================

Arrhenius-like reaction rate constants handle the formal Arrhenius equation, but include
additional temperature and pressure dependent terms that are commonly found in atmospheric
chemical mechanisms. The full equation follows:

.. math::

   A e^{-\frac{E_a}{k_bT}} \left(\frac{T}{D}\right)^B (1.0 + E \cdot P)

where:

- :math:`A` is the pre-exponential factor (:math:`(\mbox{mol}\,\mathrm{m}^{-3})^{-(n-1)}\,\mathrm{s}^{-1}`),
- :math:`n` is the number of reactants,
- :math:`E_a` is the activation energy :math:`(\mathrm{J})`,
- :math:`k_b` is the Boltzmann constant :math:`(\mathrm{J}/\mathrm{K})`,
- :math:`D` :math:`(\mathrm{K})`, :math:`B` (unitless), and :math:`E` (:math:`\mathrm{Pa}^{-1}`) are reaction parameters,
- :math:`T` is the temperature :math:`(\mathrm{K})`, and :math:`P` is the pressure :math:`(\mathrm{Pa})`.

The first two terms are described in Finlayson-Pitts and Pitts (2000) :cite:`Finlayson-Pitts2000`.
The final term is included to accommodate CMAQ EBI solver type 7 rate constants.

Input data for Arrhenius rate constants have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml
           :force:

            type: ARRHENIUS
            A: 123.45
            Ea: 123.45
            B: 1.3
            D: 300.0
            E: 6.0e-06
            gas phase: gas
            reactants:
              - species name: foo
                coefficient: 2.0
            products:
              - species name: bar
              - species name: baz
                coefficient: 1.3
            name: my_reaction
            __custom property: 32.4


    .. tab-item:: JSON

        .. code-block:: json
           :force:

            {
              "type": "ARRHENIUS",
              "A": 123.45,
              "Ea": 123.45,
              "B": 1.3,
              "D": 300.0,
              "E": 6.0e-06,
              "gas phase": "gas",
              "reactants": [
                {
                  "species name": "foo",
                  "coefficient": 2.0
                }
              ],
              "products": [
                {
                  "species name": "bar"
                },
                {
                  "species name": "baz",
                  "coefficient": 1.3
                }
              ],
              "name": "my_reaction",
              "__custom property": 32.4
            }

The key-value pairs ``reactants`` and ``products`` are required. If the ``coefficient`` is not
specified for a reactant or product, it is assumed to be 1.0.

The ``gas phase`` key is required and must be set to the name of the phase the
reaction takes place in. Each reactant and product must be present in the specified phase.

Optionally, a parameter ``C`` may be included, and is taken to equal :math:`\frac{-E_a}{k_b}`.
Note that either ``Ea`` or ``C`` may be included, but not both. When neither ``Ea`` nor ``C`` are
included, they are assumed to be 0.0. When ``A`` is not included, it is assumed to be 1.0. When
``D`` is not included, it is assumed to be 300.0 K. When ``B`` is not included, it is assumed to be
0.0. When ``E`` is not included, it is assumed to be 0.0.

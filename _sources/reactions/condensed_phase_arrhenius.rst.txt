Condensed-Phase Arrhenius Reaction
==================================

Condensed-phase Arrhenius reactions are calculated using an Arrhenius-like rate constant that takes the form:

.. math::

   A e^{-\frac{E_a}{k_bT}} \left(\frac{T}{D}\right)^B (1.0 + E \cdot P)

where:

- :math:`A` is the pre-exponential factor (:math:`[U]^{-(n-1)}\,\mathrm{s}^{-1}`),
- :math:`U` is the unit of the reactants and products, which can be :math:`M` for aqueous-phase reactions or :math:`\mathrm{mol\,m}^{-3}` for all other condensed-phase reactions,
- :math:`n` is the number of reactants,
- :math:`E_a` is the activation energy :math:`(\mathrm{J})`,
- :math:`k_b` is the Boltzmann constant :math:`(\mathrm{J}/\mathrm{K})`,
- :math:`D` :math:`(\mathrm{K})`, :math:`B` (unitless), and :math:`E` (:math:`\mathrm{Pa}^{-1}`) are reaction parameters,
- :math:`T` is the temperature :math:`(\mathrm{K})`, and :math:`P` is the pressure :math:`(\mathrm{Pa})`.

The first two terms are described in Finlayson-Pitts and Pitts (2000) :cite:`Finlayson-Pitts2000`. The final term is included to accommodate CMAQ EBI solver type 7 rate constants :cite:`Gipson`.

Input data for condensed-phase Arrhenius reactions have the following format:

.. tabs::

    .. tab:: YAML

        .. code-block:: yaml

            type: CONDENSED_PHASE_ARRHENIUS
            A: 123.45
            Ea: 123.45
            B: 1.3
            D: 300.0
            E: 0.6E-5
            units: M
            time unit: MIN
            aerosol phase: my aqueous phase
            aerosol-phase water: H2O_aq
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
                "type": "CONDENSED_PHASE_ARRHENIUS",
                "A": 123.45,
                "Ea": 123.45,
                "B": 1.3,
                "D": 300.0,
                "E": 0.6E-5,
                "units": "M",
                "time unit": "MIN",
                "aerosol phase": "my aqueous phase",
                "aerosol-phase water": "H2O_aq",
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

Units for the reactants and products must be specified using the key ``units`` and can be either ``M`` or ``mol m-3``. If units of ``M`` are specified, a key-value pair ``aerosol-phase water`` must also be included, whose value is a string specifying the name for water in the aerosol phase.

The unit for time is assumed to be s, but inclusion of the optional key-value pair ``time unit = MIN`` can be used to indicate a rate with minutes as the time unit.

The key-value pair ``aerosol phase`` is required and must specify the name of the aerosol phase in which the reaction occurs.

Optionally, a parameter ``C`` may be included, and is taken to equal :math:`\frac{-E_a}{k_b}`. Note that either ``Ea`` or ``C`` may be included, but not both. When neither ``Ea`` nor ``C`` are included, they are assumed to be 0.0. When ``A`` is not included, it is assumed to be 1.0. When ``D`` is not included, it is assumed to be 300.0 K. When ``B`` is not included, it is assumed to be 0.0. When ``E`` is not included, it is assumed to be 0.0.

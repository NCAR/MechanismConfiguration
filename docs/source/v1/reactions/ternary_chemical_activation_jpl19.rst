Ternary Chemical Activation Reaction (JPL-19)
=============================================

.. note::
   This reaction type requires version ``1.4.0`` or newer. See :ref:`v1-versions`.

JPL Evaluation 19 :cite:`JPL19` gives a different equation for ternary chemical activation
reactions than the one used by :doc:`ternary_chemical_activation`, which is based on
JPL Evaluation 18 :cite:`JPL15`. The rate constant is the sum of a Troe (fall-off) term
:math:`k_f` and a chemical activation term :math:`k_f^{CA}`:

.. math::

   k = k_f + k_f^{CA}

The Troe term is:

.. math::

   k_f = \frac{k_0[\mathrm{M}]}{1 + \frac{k_0[\mathrm{M}]}{k_{\infty}}} F_C^{\left(1 + \frac{1}{N} [\log_{10}(\frac{k_0[\mathrm{M}]}{k_{\infty}})]^2\right)^{-1}}

The chemical activation term is:

.. math::

   k_f^{CA} = k_{int} \left(1 - \frac{k_f}{k_{\infty}}\right)

where:

- :math:`k_0` is the low-pressure limiting rate constant (:math:`(\mbox{mol}\,\mathrm{m}^{-3})^{-(n-1)}\,\mathrm{s}^{-1}`),
- :math:`k_{\infty}` is the high-pressure limiting rate constant (:math:`(\mbox{mol}\,\mathrm{m}^{-3})^{-(n-1)}\,\mathrm{s}^{-1}`),
- :math:`k_{int}` is the chemical activation rate constant (:math:`(\mbox{mol}\,\mathrm{m}^{-3})^{-(n-1)}\,\mathrm{s}^{-1}`),
- :math:`[\mathrm{M}]` is the density of air (:math:`\mathrm{mol}\,\mathrm{m}^{-3}`),
- :math:`F_C` and :math:`N` are parameters that determine the shape of the fall-off curve,
  and are typically 0.6 and 1.0, respectively :cite:`JPL19`.

:math:`k_0`, :math:`k_{\infty}`, and :math:`k_{int}` each have the form of an `Arrhenius`
rate constant:

.. math::

   k_x = A \, e^{\frac{C}{T}} \left(\frac{T}{D}\right)^B

where :math:`T` is the temperature (:math:`\mathrm{K}`). Unlike :doc:`ternary_chemical_activation`,
the reference temperature :math:`D` can be set for each rate constant, and its default is
298 K, which is the reference temperature in JPL Evaluation 19.

The JPL tables give :math:`k_0 = k_0^{298} (T/298)^{-n}`, :math:`k_{\infty} = k_{\infty}^{298} (T/298)^{-m}`,
and :math:`k_{int} = A \, e^{-B/T}`. These map to the configuration parameters as:

.. list-table::
   :header-rows: 1
   :widths: 20 20 20 20 20

   * - Rate constant
     - ``_A``
     - ``_B``
     - ``_C``
     - ``_D``
   * - :math:`k_0`
     - :math:`k_0^{298}`
     - :math:`-n`
     - 0
     - 298
   * - :math:`k_{\infty}`
     - :math:`k_{\infty}^{298}`
     - :math:`-m`
     - 0
     - 298
   * - :math:`k_{int}`
     - :math:`A`
     - 0
     - :math:`-B`
     - any (no effect when ``kint_B`` is 0)

Input data for JPL-19 Ternary Chemical Activation reactions have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: TERNARY_CHEMICAL_ACTIVATION_JPL19
            name: foo-ternary-jpl19
            k0_A: 6.9e-33
            k0_B: -2.1
            k0_C: 0.0
            k0_D: 298.0
            kinf_A: 1.1e-12
            kinf_B: 1.3
            kinf_C: 0.0
            kinf_D: 298.0
            kint_A: 1.85e-13
            kint_B: 0.0
            kint_C: -65.0
            kint_D: 298.0
            Fc: 0.6
            N: 1.0
            gas phase: gas
            reactants:
                - species name: foo
                - species name: bar
            products:
                - species name: baz
                - species name: qux
                  coefficient: 0.65


    .. tab-item:: JSON

        .. code-block:: json

            {
              "type": "TERNARY_CHEMICAL_ACTIVATION_JPL19",
              "name": "foo-ternary-jpl19",
              "k0_A": 6.9e-33,
              "k0_B": -2.1,
              "k0_C": 0.0,
              "k0_D": 298.0,
              "kinf_A": 1.1e-12,
              "kinf_B": 1.3,
              "kinf_C": 0.0,
              "kinf_D": 298.0,
              "kint_A": 1.85e-13,
              "kint_B": 0.0,
              "kint_C": -65.0,
              "kint_D": 298.0,
              "Fc": 0.6,
              "N": 1.0,
              "gas phase": "gas",
              "reactants": [
                {
                  "species name": "foo"
                },
                {
                  "species name": "bar"
                }
              ],
              "products": [
                {
                  "species name": "baz"
                },
                {
                  "species name": "qux",
                  "coefficient": 0.65
                }
              ]
            }

The key-value pairs ``reactants`` and ``products`` are required. When a ``coefficient`` is not
specified for a reactant or product, it is assumed to be 1.0.

The ``gas phase`` key is required and must be set to the name of the phase the reaction
takes place in. The reactants and products must be present in the specified phase.

The three sets of parameters beginning with ``k0_``, ``kinf_``, and ``kint_`` are the parameters for the
:math:`k_0`, :math:`k_{\infty}`, and :math:`k_{int}` rate constants, respectively. When not present,
``_A`` parameters are assumed to be 1.0, ``_B`` to be 0.0, ``_C`` to be 0.0, ``_D`` to be 298.0,
``Fc`` to be 0.6, and ``N`` to be 1.0.

Rate constants are in units of :math:`\mathrm{(m^{3}\ mol^{-1})^{(n-1)}\ s^{-1}}` where :math:`n` is the total number of reactants.

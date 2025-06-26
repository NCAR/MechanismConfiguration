SIMPOL.1 Phase-Transfer Reaction
================================

SIMPOL phase transfer reactions are based on the SIMPOL.1 model calculations of vapor pressure described by Pankow and Asher (2008) :cite:`Pankow2008`.

Mass accommodation coefficients and condensation rate constants are calculated using the method of Ervans et al. (2003) :cite:`Ervens2003` and references therein. Mass accommodation coefficients (:math:`\alpha`) are calculated as:

.. math::

   \Delta H_{obs} = -10 \times (N^*-1) + 7.53 \times (N^{*2/3}-1) - 0.1 \times 10 \quad (\text{kcal}\,\text{M}^{-1})

.. math::

   \Delta S_{obs} = -13 \times (N^*-1) - 19 \times (N^*-1) + 9.21 \times (N^{*2/3}-1) - 0.1 \times 13 \quad (\text{cal}\,\text{M}^{-1}\,\text{K}^{-1})

.. math::

   \frac{\alpha}{1-\alpha} = e^{\frac{-\Delta G^{*}}{RT}}

If :math:`\Delta H` and :math:`\Delta S` are not provided, the mass accommodation coefficient is assumed to be 0.1 :cite:`Zaveri2008`.

Condensation rate constants are calculated following :cite:`Zaveri2008` as:

.. math::

   k_c = 4 \pi r D_g f_{fs}( K_n, \alpha )

where :math:`r` is the radius of the particle(s) [m], :math:`D_g` is the diffusion coefficient of the gas-phase species :math:`[\mathrm{m}^2\,\mathrm{s}^{-1}]`,
:math:`f_{fs}( K_n, \alpha )` is the Fuchs Sutugin transition regime correction factor [unitless], :math:`K_n` is the Knudsen Number [unitless], and :math:`\alpha` is the mass accommodation coefficient.

Rates can be calculated as:

.. math::

   r_c = [G] N_a k_c

where :math:`[G]` is the gas-phase species concentration [ppm], :math:`N_a` is the number concentration of particles 
:math:`[\mathrm{particle}\,\mathrm{m}^{-3}]`, and the rate :math:`r_c` is in :math:`[\mathrm{ppm}\,\mathrm{s}^{-1}]`.

The particle radius used to calculate :math:`k_{f}` is the effective radius [:math:`r_{eff}`], which is taken as the "least-wrong" choice for condensation rates, as it is weighted to surface area :cite:`Zender2002`.

Input data for SIMPOL phase transfer reactions have the following format:

.. tabs::

    .. tab:: YAML

        .. code-block:: yaml

            type: SIMPOL_PHASE_TRANSFER
            gas-phase species: my gas spec
            aerosol phase: my aero phase
            aerosol-phase species: my aero spec
            aerosol-phase activity coefficient: my aero act coeff
            B: [ 123.2e3, -41.24, 2951.2, -1.245e-4 ]
            ...

    .. tab:: JSON

        .. code-block:: json

            {
                "type": "SIMPOL_PHASE_TRANSFER",
                "gas-phase species": "my gas spec",
                "aerosol phase": "my aero phase",
                "aerosol-phase species": "my aero spec",
                "aerosol-phase activity coefficient": "my aero act coeff",
                "B": [ 123.2e3, -41.24, 2951.2, -1.245e-4 ],
            }

The key-value pairs ``gas-phase species``, ``aerosol phase``, and ``aerosol-phase species`` are required. 
Only one gas-phase and one aerosol-phase species are allowed per phase-transfer reaction. 
The key-value pair ``aerosol-phase activity coefficient`` is optional. 
When included, its value must be the name of a species of type \c ACTIVITY_COEFF that is present in the specified aerosol phase. 
When not included, activity coefficients are assumed to be 1.0.

Gas-phase species must include parameters named ``diffusion coeff [m2 s-1]`` , which specifies the diffusion coefficient in :math:`\mathrm{m}^2\,\mathrm{s}^{-1}`,
and ``molecular weight [kg mol-1]``, which specifies the molecular weight of the species in :math:`[\mathrm{kg}\,\mathrm{mol}^{-1}]`. 
They may optionally include the parameter ``N star``, which will be used to calculate the mass accommodation coefficient. When this parameter is not included, the mass accommodation coefficient is assumed to be 1.0.

The key-value pair ``B`` is also required and must have a value of an array of exactly four members that specifies the SIMPOL parameters for the partitioning species. The ``B`` parameters can be obtained by summing the contributions of each functional group present in the partitioning species to the overall :math:`B_{n,i}` for species :math:`i`, such that:

.. math::

   B_{n,i} = \sum_{k} \nu_{k,i} B_{n,k} \forall n \in [1...4]

where :math:`\nu_{k,i}` is the number of functional groups :math:`k` in species :math:`i`, and the parameters :math:`B_{n,k}` for each functional group :math:`k` can be found in table 5 of Pankow and Asher (2008) :cite:`Pankow2008`.

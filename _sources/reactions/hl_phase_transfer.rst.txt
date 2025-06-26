Henry's Law Phase-Transfer Reaction
===================================

Henry's Law phase-transfer reactions use equilibrium rate constants that are calculated as:

.. math::

   HLC(298K) \cdot e^{C({1/T-1/298})}

where :math:`HLC(298K)` is the Henry's Law constant at 298 K [M\,Pa^{-1}], :math:`C` is a constant [K], and :math:`T` is the temperature [K]. Uptake kinetics are based on the particle effective radius, :math:`r_{eff}` [m], the condensing species gas-phase diffusion coefficient, :math:`D_g` [m^2\,s^{-1}], its molecular weight :math:`MW` [kg\,mol^{-3}], and :math:`N^{*}`, which is used to calculate the mass accommodation coefficient.

Mass accommodation coefficients and condensation rate constants are calculated using the method of Ervans et al. (2003) [Ervens2003] and references therein. Mass accommodation coefficients (:math:`\alpha`) are calculated as:

.. math::

   \Delta H_{obs} = -10 \times (N^*-1) + 7.53 \times (N^{*2/3}-1) - 0.1 \times 10 \quad (\text{kcal}\,\text{M}^{-1})
   

.. math::

   \Delta S_{obs} = -13 \times (N^*-1) - 19 \times (N^*-1) + 9.21 \times (N^{*2/3}-1) - 0.1 \times 13 \quad (\text{cal}\,\text{M}^{-1}\,\text{K}^{-1})
   
.. math::

   \frac{\alpha}{1-\alpha} = e^{\frac{-\Delta G^{*}}{RT}}

If :math:`\Delta H` and :math:`\Delta S` are not provided, :math:`\alpha` is set to 0.1 [Zaveri2008].

Condensation rate constants are calculated following [Zaveri2008] as:

.. math::

   k_c = 4 \pi r D_g f_{fs}( K_n, \alpha )

where :math:`r` is the radius of the particle(s) [m], :math:`D_g` is the diffusion coefficient of the gas-phase species [m^2\,s^{-1}], :math:`f_{fs}( K_n, \alpha )` is the Fuchs Sutugin transition regime correction factor [unitless], :math:`K_n` is the Knudsen Number [unitless], and :math:`\alpha` is the mass accommodation coefficient.

Rates can be calculated as:

.. math::

   r_c = [G] N_a k_c

where :math:`[G]` is the gas-phase species concentration [ppm], :math:`N_a` is the number concentration of particles [particle\,m^{-3}], and the rate :math:`r_c` is in [ppm\,s^{-1}]. The particle radius used to calculate :math:`k_f` is the effective radius [:math:`r_{eff}`], which is taken as the "least-wrong" choice for condensation rates, as it is weighted to surface area [Zender2002].

Input data for Phase transfer equations have the following format:

.. tabs::

    .. tab:: YAML

        .. code-block:: yaml

            type: HL_PHASE_TRANSFER
            gas-phase species: my gas spec
            aerosol-phase species: my aero spec
            aerosol phase: my aqueous phase
            aerosol-phase water: H2O_aq
            ...

    .. tab:: JSON

        .. code-block:: json

            {
                "type": "HL_PHASE_TRANSFER",
                "gas-phase species": "my gas spec",
                "aerosol-phase species": "my aero spec",
                "aerosol phase": "my aqueous phase",
                "aerosol-phase water": "H2O_aq",
            }

The key-value pairs ``gas-phase species`` and ``aerosol-phase species`` are required. 
Only one gas-phase and one aerosol-phase species are allowed per phase-transfer reaction.
Additionally, gas-phase species must include parameters named ``HLC(298K)`` :math:`[\mathrm{M}\ \mathrm{Pa}^{-1}]`,
which is the Henry's Law constant at 298 K, ``HLC exp factor`` :math:`[\mathrm{K}]`, which is the Henry's Law constant exponential factor ``C``, 
``diffusion coeff [m2 s-1]`` which specifies the diffusion coefficient in 
:math:`\mathrm{m}^2\,\mathrm{s}^{-1}`, and ``molecular weight``, which specifies the molecular weight of the species in :math:`\mathrm{kg}\,\mathrm{mol}^{−1}`.
They may optionally include the parameter ``N star``, which will be used to calculate the mass accomodation coefficient.
When this parameter is not included, the mass accomodation coefficient is assumed to be 1.0.

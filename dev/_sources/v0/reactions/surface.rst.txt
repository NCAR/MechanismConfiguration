Surface (Heterogeneous) Reaction
================================

Surface reactions transform gas-phase species into gas-phase products according to a rate that is calculated based on the total exposed surface area of a condensed phase.

For surface reactions of the gas-phase species X, the reaction rate is calculated assuming large particles (continuum regime) as:

.. math::

   r_{surface} = k_{surface}[X]

where ``[X]`` is the gas-phase concentration of species ``X`` :math:`[\mathrm{molecule}\,\mathrm{cm}^{-3}]`, and the rate constant :math:`(k_{\mathrm{surface}}) [\mathrm{s}^{-1}]` is calculated as:

.. math::

   k_{surface} = \frac{4N_a \pi r_e^2}{\left(\frac{r_e}{D_g} + \frac{4}{v(T)\gamma}\right)}

where ``N_a`` is the number concentration of particles :math:`[\mathrm{particles}\,\mathrm{m}^{-3}]`, :math:`r_e` is the effective particle radius :math:`[\mathrm{m}]`, 
``D_g`` is the gas-phase diffusion coefficient of the reactant :math:`[\mathrm{m}^2\mathrm{s}^{-1}]`, ``gamma`` is the reaction probability [unitless], and ``v`` is the mean free speed of the gas-phase reactant:

.. math::

   v = \sqrt{\frac{8RT}{\pi MW}}

where ``R`` is the ideal gas constant :math:`[\mathrm{J} \mathrm{K}^{-1} \mathrm{mol}^{-1}]`, ``T`` is temperature :math:`[\mathrm{K}]`, and ``MW`` is the molecular weight of the gas-phase reactant :math:`[\mathrm{kg}\ \mathrm{mol}^{-1}]`.

A conforming implementation of version |version| of the configuration will provide ways to specify the particle effective radius and number concentration.

Input data for surface reactions have the following format:

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml

         type : SURFACE
         gas-phase reactant : my gas species
         reaction probability : 0.2
         gas-phase products :
            my other gas species :
            another gas species : { "yield" : 0.3 }


   .. tab-item:: JSON

      .. code-block:: json

         {
           "type" : "SURFACE",
           "gas-phase reactant" : "my gas species",
           "reaction probability" : 0.2,
           "gas-phase products" : {
              "my other gas species" : { },
              "another gas species" : { "yield" : 0.3 }
           }
         }

The key-value pairs ``gas-phase reactant`` and ``reaction probability`` are required. 
Only one gas-phase reactant is allowed, but multiple products can be present. 
The key-value pair ``yield`` for product species is optional and defaults to 1.0.

The gas-phase reactant species must include properties ``diffusion coeff [m^2 s-1]``,
which specifies the diffusion coefficient in :math:`\mathrm{m}^2\ \mathrm{s}^{-1}`, and ``molecular weight [kg mol-1]``,
which specifies the molecular weight of the species in :math:`\mathrm{kg}\ \mathrm{mol}^{-1}`.

.. _v0-reactions:

#########
Reactions
#########

All of the reactions list below are implemented in `MICM <https://github.com/NCAR/micm>`_. Their configuration is defined in 
this repository, as are the algorithms that micm implements.

* :doc:`arrhenius` - :cpp:struct:`micm::ArrheniusRateConstantParameters`
* :doc:`branched` - :cpp:struct:`micm::BranchedRateConstantParameters`
* :doc:`emission` - :cpp:struct:`micm::UserDefinedRateConstantParameters`
* :doc:`first_order_loss` - :cpp:struct:`micm::UserDefinedRateConstantParameters`
* :doc:`photolysis` - :cpp:struct:`micm::UserDefinedRateConstantParameters`
* :doc:`surface` - :cpp:struct:`micm::SurfaceRateConstantParameters`
* :doc:`ternary_chemical_activation` - :cpp:struct:`micm::TernaryChemicalActivationRateConstantParameters`
* :doc:`troe` - :cpp:struct:`micm::TroeRateConstantParameters`
* :doc:`tunneling` - :cpp:struct:`micm::TunnelingRateConstantParameters`



.. toctree::
   :maxdepth: 2
   :caption: Contents:

   arrhenius
   branched
   emission
   first_order_loss
   photolysis
   surface
   taylor_series
   ternary_chemical_activation
   troe
   tunneling

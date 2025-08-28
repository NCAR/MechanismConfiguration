.. _v1-reactions:

#########
Reactions
#########

All of the reactions listed below are implemented in `MICM <https://github.com/NCAR/micm>`_.
Their configuration is defined in this repository, as are the algorithms that micm implements.

* :doc:`arrhenius` - :cpp:class:`micm::ArrheniusRateConstant`
* :doc:`branched` - :cpp:class:`micm::BranchedRateConstant`
* :doc:`emission` - :cpp:class:`micm::UserDefinedRateConstant`
* :doc:`first_order_loss` - :cpp:class:`micm::UserDefinedRateConstant`
* :doc:`photolysis` - :cpp:class:`micm::UserDefinedRateConstant`
* :doc:`surface` - :cpp:class:`micm::SurfaceRateConstant`
* :doc:`taylor_series` - :cpp:class:`micm::TaylorSeriesRateConstant`
* :doc:`ternary_chemical_activation` - :cpp:class:`micm::TernaryChemicalActivationRateConstant`
* :doc:`troe` - :cpp:class:`micm::TroeRateConstant`
* :doc:`tunneling` - :cpp:class:`micm::TunnelingRateConstant`
* :doc:`user_defined` - :cpp:class:`micm::UserDefinedRateConstant`



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
   user_defined

.. _reactions:

#########
Reactions
#########

These reactions are implemented in a few chemical solvers that read version |version| of the mechanism configuraiton.

====================================== ============================================= ======================================= ===============================================
Reaction Type                          `CAMP <https://github.com/open-atmos/camp>`_  `MICM <https://github.com/NCAR/micm>`_  `TCHEM <https://github.com/sandialabs/TChem>`_ 
====================================== ============================================= ======================================= ===============================================
arrhenius                              |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
branched                               |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
emission                               |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
first_order_loss                       |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
photolysis                             |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
surface                                |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
ternary_chemical_activation            |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
troe                                   |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
tunneling                              |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
====================================== ============================================= ======================================= ===============================================



.. toctree::
   :maxdepth: 2
   :caption: Contents:

   arrhenius
   branched
   emission
   first_order_loss
   photolysis
   surface
   ternary_chemical_activation
   troe
   tunneling

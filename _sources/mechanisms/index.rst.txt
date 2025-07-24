.. _mechanisms:

##########
Mechanisms
##########

Mechanisms are collections of reactions and have the following format:

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         camp-data:
           - name: my mechanism
             type: MECHANISM 
             reactions:
               - type: RXN_ARRHENIUS
                 ...
               - type: RXN_TROE
                 ...

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "camp-data": [
             "name": "my mechanism",
             "type": "MECHANISM",
             "reactions": [
               {
                 "type": "RXN_ARRHENIUS",
                 ...
               },
               {
                 "type": "RXN_TROE",
                 ...
               }
             ]
           ]


The ``name`` is required and must be unique.
Mechanisms with the same name will be merged into a single mechanism.
The ``type`` is also required and must be ``MECHANISM``.
The ``reactions`` should be an array of :ref:`reactions`.

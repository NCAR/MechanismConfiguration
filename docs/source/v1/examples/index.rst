##########
Examples
##########

.. toctree::
   :maxdepth: 2
   :caption: Contents:


Chapman
=======

Top Level Config
----------------

.. raw:: html

    <div class="download-div">
    <a href="../../_static/examples/v1/yaml/chapman/chapman.zip" download>
       <button class="download-button">Download yaml ZIP</button>
    </a>
    <a href="../../_static/examples/v1/json/chapman/chapman.zip" download>
       <button class="download-button">Download json ZIP</button>
    </a>
    </div>
    

.. tab-set::

    .. tab-item:: YAML

        .. literalinclude:: ../../_static/examples/v1/yaml/chapman/config.yaml
            :language: yaml

    .. tab-item:: JSON

        .. literalinclude:: ../../_static/examples/v1/json/chapman/config.json
            :language: json
    

CAM Cloud Chemistry
===================

This example has aerosol representations, processes, and constraints. It describes
the sulfur chemistry in cloud droplets that CAM uses. See :ref:`v1-aerosol`.

.. literalinclude:: ../../../../examples/v1/cam_cloud_chemistry.json
    :language: json

##########
Examples
##########

.. toctree::
   :maxdepth: 2
   :caption: Contents:


Examples for Each Version
=========================

The ``examples/v1`` directory of the
`repository <https://github.com/NCAR/MechanismConfiguration/tree/main/examples/v1>`_
has a complete example for each minor version (see :ref:`v1-versions`). Each example has
a YAML and a JSON form. The tests parse each of these examples.

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Directory
     - Content
   * - ``examples/v1/1.0``
     - An inline configuration with every gas-phase reaction type
   * - ``examples/v1/1.1``
     - The 1.0 mechanism, split into species, phases, and reactions files with the file-list format
   * - ``examples/v1/1.2``
     - The 1.0 mechanism with an aerosol section that has every representation, process, and constraint type
   * - ``examples/v1/1.3``
     - The 1.2 mechanism with an emissions section

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

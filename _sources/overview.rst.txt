########
Overview
########

Chemical mechnanisms can be organized across a number of files in
whatever way makes sense for the specific user and application.
Every configuration should include one top-level file that
lists the relative or absolute path to every configuration
file that includes mechanism configuration data.

Individual mechanism configuration files are combined to
collectively describe the chemical system and processes.
The order of files in the list is arbitrary.

The file list configuration has the following format:

.. tab-set::

  .. tab-item:: YAML

    .. code-block:: yaml

        camp-files:
          - path/to/foo.yml
          - path/to/bar.yml

  .. tab-item:: JSON

    .. code-block:: json

      {
        "camp-files": [
          "path/to/foo.json",
          "path/to/bar.json"
        ]
      }


Individual configuration files have the following format:

.. tab-set::

  .. tab-item:: YAML

    .. code-block:: yaml
       :force:

        camp-data:
          - type: OBJECT_TYPE
            ...
          - type: OBJECT_TYPE
            ...

  .. tab-item:: JSON

    .. code-block:: json
       :force:

        {
          "camp-data": [
            {
              "type": "OBJECT_TYPE",
              ...
            },
            {
              "type": "OBJECT_TYPE",
              ...
            }
          ]
        }


Each input file should contain exactly one object with a single key-value
pair ``camp-data`` whose value is an array of objects.
Additional top-level key-value pairs will be ignored.
Each of the objects in the ``camp-data`` array must contain a key-value pair ``type``
whose value is a string referencing a valid configuration class.

The valid top-level configuration objects that can appear in the ``camp-data``
array are described here:

:ref:`chemical-species`

:ref:`mechanisms`

The arrangement of objects within the ``camp-data`` array and between input files is arbitrary.
Additionally, some objects, such as chemical species and mechanisms may be split into
multiple objects within the ``camp-data`` array and/or between files,
and will be combined based on their unique name.
This flexibility is provided so that the chemical mechanism data can be organized
in a way that makes sense to the designer of the mechanism.
For example, files could be split based on species source (biogenic, fossil fuel, etc.)
or based on properties (molecular weight, density, etc.) or any combination of criteria.
However, if a single property of an object (e.g., the molecular weight of a chemical species)
is set in more than one location, this should cause an error.
.. nsf ncar mechanism configuration documentation HTML titles
..
.. # (over and under) for module headings
.. = for sections
.. - for subsections
.. ^ for subsubsections
.. ~ for subsubsubsections
.. " for paragraphs

###############################################################
Welcome to the NSF NCAR Mechanism Configuration documentation!
###############################################################

Mechanism Configuration is a model-independent configuration schema for atmospheric
chemical systems in JSON and YAML. This repository also has a C++ library that parses
and validates these configurations.

.. grid:: 1 1 2 2
    :gutter: 2

    .. grid-item-card:: Getting started
        :img-top: _static/index_getting_started.svg
        :link: getting_started
        :link-type: doc

        Build and install the library, then parse your first mechanism.

    .. grid-item-card::  User guide
        :img-top: _static/index_user_guide.svg
        :link: user_guide/index
        :link-type: doc

        Learn how to write the species, phases, and reactions of a mechanism.

    .. grid-item-card::  API reference
        :img-top: _static/index_api.svg
        :link: api/index
        :link-type: doc

        The C++ types and functions that parse and validate a mechanism.

    .. grid-item-card::  Contributors guide
        :img-top: _static/index_contribute.svg
        :link: contributing/index
        :link-type: doc

        Add a new reaction type, update the docs, or fix a bug.


.. toctree::
   :maxdepth: 2
   :caption: Contents:

   getting_started
   user_guide/index
   api/index
   contributing/index
   citing_and_bibliography/index
   changelog

Indices and tables
==================

* :ref:`genindex`
* :ref:`search`

.. _Contributing:

Contributing
============

For all proposed changes (bug fixes, new features, documentation updates, and others), please file an
`issue <https://github.com/NCAR/MechanismConfiguration/issues/new>`_ first. In the issue, tell us
what you need or what you intend to do.

The NSF NCAR software developers will work with you on that issue. We can recommend an implementation,
answer questions, or give other background knowledge.

Testing
-------

All code that you contribute must have unit tests. Our tests run automatically on Linux, macOS, and
Windows, with different compilers. Tests help us find platform problems early.

Each test that reads a configuration runs on the JSON and the YAML form of that configuration.
The test configurations are in ``test/unit`` and ``test/integration``. The tests also parse each
configuration in the ``examples`` directory.

The project collects code coverage statistics. Our homepage shows them in a badge:

.. image:: https://codecov.io/gh/NCAR/MechanismConfiguration/branch/main/graph/badge.svg
    :target: https://codecov.io/gh/NCAR/MechanismConfiguration
    :alt: codecov badge

The codecov bot also reports the coverage on most pull requests. The code that you add must not
make the coverage decrease.

Add a reaction type
-------------------

A new reaction type touches these parts of the repository:

#. Add a struct to ``include/mechanism_configuration/types/reactions.hpp`` and add a vector of it to ``types::Reactions``.
#. Add the type key and any new parameter keys to ``src/detail/v1/reactions/keys.hpp``.
#. Add a parser class to ``src/detail/v1/reactions/parsers.hpp`` and register it in the parser map.
#. Add the parser source file to ``src/v1/reactions`` and to ``src/v1/reactions/CMakeLists.txt``.
#. Add the type to ``Validate()`` in ``src/validate.cpp``.
#. Add unit tests and test configurations to ``test/unit/v1/reactions``.
#. Add the type to the configurations in ``examples/v1``.
#. Add a page to ``docs/source/v1/reactions`` and add it to the reactions index.
#. Add an entry to ``docs/source/changelog.rst``.

Style guide
-----------

We (mostly) follow the `Google C++ style guide <https://google.github.io/styleguide/cppguide.html>`_.
Please try to do the same. This decreases the number of comments on your pull request. We are not
dogmatic, and we accept reasonable exceptions, especially when they make code or an API simpler.

After we merge a pull request, a GitHub action runs ``clang-format`` on the code. The ``.clang-format``
file in the repository root has the configuration. You can run ``clang-format`` before you commit,
but you do not have to.

Building the documentation
--------------------------

The documentation is in the ``docs`` directory. The ``docs/requirements.txt`` file lists the Python
packages that the documentation needs. The API reference also needs `Doxygen <https://www.doxygen.nl/>`_.

CMake runs Doxygen and then Sphinx. From the root directory of the repository:

Venv
^^^^

.. code-block:: bash

  python -m venv mc_env
  source mc_env/bin/activate
  pip install -r docs/requirements.txt
  mkdir build && cd build
  cmake -DMECH_CONFIG_BUILD_DOCS=ON ..
  make docs
  open docs/sphinx/index.html

Conda
^^^^^

.. code-block:: bash

  conda create --name mc_env python -y
  conda activate mc_env
  pip install -r docs/requirements.txt
  mkdir build && cd build
  cmake -DMECH_CONFIG_BUILD_DOCS=ON ..
  make docs
  open docs/sphinx/index.html

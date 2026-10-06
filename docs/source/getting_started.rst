###############
Getting Started
###############

Build and Test
==============

To build and install the library, you must have these tools:

- `CMake <https://cmake.org/>`_ 3.24 or newer
- A C++ compiler that supports C++23, because the library uses ``std::expected``

.. note::
   On Linux, clang cannot compile ``std::expected`` with libstdc++. Use GCC, or use clang with libc++.

CMake downloads the other dependencies (yaml-cpp, and googletest for the tests).

.. code-block:: console

    $ git clone https://github.com/NCAR/MechanismConfiguration.git
    $ cd MechanismConfiguration
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make install -j 8
    $ make test

To change the installation directory, set ``CMAKE_INSTALL_PREFIX``.

Options
-------

You can set these CMake options with ``-D<OPTION>=<VALUE>`` or with ``ccmake``.

.. list-table::
   :header-rows: 1
   :widths: 40 10 50

   * - Option
     - Default
     - Description
   * - ``MECH_CONFIG_ENABLE_TESTS``
     - ``ON``
     - Build the tests
   * - ``MECH_CONFIG_BUILD_SHARED_LIBS``
     - ``OFF``
     - Build a shared library instead of a static library
   * - ``MECH_CONFIG_BUILD_DOCS``
     - ``OFF``
     - Build this documentation. See :ref:`Contributing`.
   * - ``MECH_CONFIG_ENABLE_COVERAGE``
     - ``OFF``
     - Make a code coverage report from the tests
   * - ``MECH_CONFIG_USE_FMT``
     - ``OFF``
     - Use the {fmt} library instead of ``std::format``
   * - ``MECH_CONFIG_COMPILE_WARNING_AS_ERROR``
     - ``OFF``
     - Treat compiler warnings as errors

Use the library in a CMake project
----------------------------------

After you install the library, find it with ``find_package`` and link to the ``musica::mechanism_configuration`` target:

.. code-block:: cmake

    find_package(mechanism_configuration REQUIRED)
    target_link_libraries(my_target PRIVATE musica::mechanism_configuration)

Docker Container
----------------

Build and run the image::

    $ docker build -t mechanism_configuration -f docker/Dockerfile .
    $ docker run --rm -it mechanism_configuration

Run an Example
==============

All work goes through the ``mechanism_configuration::Mechanism`` struct. You can get a ``Mechanism`` in two ways:

- **Parse** a configuration file. The parser reads the version from the file and uses the correct parser for it.
- **Build** a ``Mechanism`` in code, then **validate** it.

``Validate()`` runs the same semantic checks as the parser. For example, it checks that each species in a reaction exists.

.. code-block:: cpp

    #include <mechanism_configuration/mechanism_configuration.hpp>

    #include <iostream>

    using namespace mechanism_configuration;

    void print_errors(const Errors& errors)
    {
      for (const auto& [code, message] : errors)
        std::cerr << "  [" << ErrorCodeToString(code) << "] " << message << '\n';
    }

    int main()
    {
      int status = 0;

      // 1) Parse from a file (YAML or JSON; v0 or v1). Returns std::expected<Mechanism, Errors>
      //    with both structural and semantic errors reported.
      if (auto parsed = Parse("examples/v1/full_configuration.yaml"))
      {
        const Mechanism& mechanism = *parsed;
        std::cout << "Parsed '" << mechanism.name << "': " << mechanism.species.size()
                  << " species, " << mechanism.reactions.arrhenius.size() << " Arrhenius reactions\n";
      }
      else
      {
        std::cerr << "Failed to parse file:\n";
        print_errors(parsed.error());
        status = 1;
      }

      // 2) Build a Mechanism in code and validate it (species exist, reactants are in their
      //    phase, no duplicate names, ...).
      Mechanism mechanism;
      mechanism.name = "example";
      mechanism.species = { { .name = "A" }, { .name = "B" } };
      mechanism.phases = { { .name = "gas", .species = { { .name = "A" }, { .name = "B" } } } };

      types::Arrhenius reaction;
      reaction.name = "A -> B";
      reaction.gas_phase = "gas";
      reaction.reactants = { { .name = "A" } };  // reactants must be registered in the phase
      reaction.products = { { .name = "B" } };   // products may reference any phase
      mechanism.reactions.arrhenius = { reaction };

      if (Errors errors = Validate(mechanism); errors.empty())
        std::cout << "In-code mechanism is valid\n";
      else
      {
        std::cerr << "In-code mechanism is invalid:\n";
        print_errors(errors);
        status = 1;
      }

      return status;
    }

To learn how to write a configuration file, see the :doc:`user_guide/index`.
For all of the types and functions, see the :doc:`api/index`.

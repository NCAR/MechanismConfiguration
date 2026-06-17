Changelog
=========

Version 2.0.0
-------------

Version 2.0.0 introduces breaking changes to the C++ API compared to the 1.x series.
The configuration file formats (``v0`` and ``v1``) are unchanged; the breaking changes
are in how the library is consumed from C++.

API Changes
^^^^^^^^^^^

- Parsing is now a free function that returns ``std::expected``:
  ``std::expected<Mechanism, Errors> Parse(const std::filesystem::path&)``.
  The previous ``UniversalParser`` / ``ParserResult<GlobalMechanism>`` interface has been removed.
- A version-neutral ``Errors Validate(const Mechanism&)`` has been added so an in-code
  ``Mechanism`` can be checked with the same semantic rules the parser uses.
- A single canonical ``mechanism_configuration::Mechanism`` (with ``mechanism_configuration::types::*``)
  replaces the version-specific ``v0::types::Mechanism`` and ``v1::types::Mechanism``.
- Errors are reported through ``Errors`` (a list of ``{ErrorCode, std::string}``) and
  ``ErrorCodeToString``; the previous parse-status helpers have been removed.

Header Reorganization
^^^^^^^^^^^^^^^^^^^^^^

- A single public umbrella header is now provided. Including
  ``<mechanism_configuration/mechanism_configuration.hpp>`` is sufficient to use the library.
- The public surface is limited to ``parse.hpp``, ``validate.hpp``, ``mechanism.hpp``,
  ``types.hpp``, ``errors.hpp``, and ``version.hpp`` (plus the umbrella).
- Internal headers (the ``v0`` / ``v1`` parsers, schema and validation helpers) have moved
  into a private ``detail`` tree and are no longer installed. Code that included headers such
  as ``parser.hpp``, ``parser_result.hpp``, ``parse_status.hpp``, ``error_location.hpp``, or the
  versioned ``v1/types.hpp`` / ``v1/mechanism.hpp`` must migrate to the public headers above.

Removed Reactions
^^^^^^^^^^^^^^^^^

The following reaction types were removed from the ``v1`` mechanism:

- ``AQUEOUS_EQUILIBRIUM``
- ``CONDENSED_PHASE_ARRHENIUS``
- ``CONDENSED_PHASE_PHOTOLYSIS``
- ``HENRYS_LAW``
- ``SIMPOL_PHASE_TRANSFER``
- ``WET_DEPOSITION``

Other Changes
^^^^^^^^^^^^^

- Reaction components accept the legacy ``species name`` key as an alias for ``name``.
- A ``v1`` configuration may split ``species``, ``phases``, and ``reactions`` across multiple
  files using the ``files`` list form (minor version 1.1 or later).
- Products may reference a species in any phase; only reactants must belong to the
  reaction's phase.

Migration
^^^^^^^^^

Old approach (1.x):

.. code-block:: cpp

    #include <mechanism_configuration/parser.hpp>

    using namespace mechanism_configuration;

    UniversalParser parser;
    ParserResult<GlobalMechanism> result = parser.Parse("config.yaml");
    if (result)
    {
      GlobalMechanism& mechanism = *result;
      // ...
    }
    else
    {
      for (const auto& error : result.errors)
      {
        // ...
      }
    }

New approach (2.0):

.. code-block:: cpp

    #include <mechanism_configuration/mechanism_configuration.hpp>

    using namespace mechanism_configuration;

    if (auto parsed = Parse("config.yaml"))
    {
      const Mechanism& mechanism = *parsed;
      // ...
    }
    else
    {
      for (const auto& [code, message] : parsed.error())
      {
        // ...
      }
    }

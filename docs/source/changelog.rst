Changelog
=========

Unreleased
----------

Parse Results with Partial Mechanisms
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``Parse()`` and ``ParseFromString()`` now build as much of a mechanism as possible. They return
a ``ParseResult`` with the ``mechanism`` and all the ``errors``, not ``std::expected``. An editor
can use the result to import a configuration that has problems, and to show the problems on the
imported items.

- **Breaking change:** ``ParseResult`` replaces ``std::expected<Mechanism, Errors>``. Use
  ``result.mechanism`` in place of ``*result`` and ``result.errors`` in place of
  ``result.error()``.
- The result converts to ``true`` only when it has a mechanism and no errors. A program that
  solves a mechanism must use it only then.
- ``mechanism`` is empty only when the parser cannot read the configuration, for example when the
  file does not exist, or the version is missing or not supported.
- The parser checks all the sections, and then builds every item that it can, also an item with a
  schema error. For example, a reaction with a non-standard key is kept, and the key is reported.
- An item that cannot be built is left out, with an ``ItemLeftOut`` error at its path. Examples
  are a species without a name, and a reaction with an unknown type.
- An aerosol or emissions section is built as a whole, because its items refer to each other.
- A missing ``species``, ``phases``, or ``reactions`` section is reported, and is empty.
- The semantic checks run on the items that are built, also when schema errors exist. A reference
  to an item that was left out gives a semantic error, for example ``Unknown species``. Before,
  the semantic checks ran only when there were no schema errors.
- A ``v0`` configuration also gives its partial mechanism. A reaction with an unknown type is left
  out with an ``UnknownType`` error. Before, it stopped the parse of the rest of its file.
- ``ErrorCode`` has the new value ``ItemLeftOut``.

JSON Pointer Paths in Errors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each error now tells which object in the configuration causes it. A program, for example an
editor, can use this to find the object without a parse of the message text.

- ``Errors`` is now a list of ``Error`` structs, not a list of ``std::pair``. An ``Error`` has a
  ``code``, a ``message``, and an optional ``path``. This is a breaking change: replace ``.first``
  with ``.code`` and ``.second`` with ``.message``, and use three names in a structured binding.
- ``path`` is a JSON Pointer (`RFC 6901 <https://www.rfc-editor.org/rfc/rfc6901>`_) into the
  document. ``Parse()`` and ``ParseFromString()`` set it for ``v1`` errors.
- A semantic error points to the reference that causes it, for example
  ``/reactions/3/products/0`` for a product that is not in the gas phase of the reaction.
- A schema error points to the item of a top-level list that contains the problem, for example
  ``/reactions/3``, or to the section or the document when no item contains it.
- ``Validate()`` on a ``Mechanism`` built in code gives paths into the struct for species,
  phases, and gas-phase reactions, for example ``/reactions/arrhenius/1/products/0``. The
  aerosol and emissions errors of such a ``Mechanism`` have no path.
- ``path`` is empty for ``v0`` configurations and for errors that are not about one item, such
  as a YAML syntax error.
- The message does not change, so the ``file:line:col`` prefix still opens the file at the error.

Surface Molecular Weight Check
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``Validate()`` now checks that the gas-phase species of each ``SURFACE`` reaction has a molecular
weight. MICM needs it for the mean molecular speed. A configuration without it gives a
``RequiredKeyNotFound`` error. The examples now set a molecular weight for that species.

Product Phase Check
^^^^^^^^^^^^^^^^^^^

``Validate()`` now checks that each product is in the phase of its reaction, as it does for
reactants. A configuration with a product outside the reaction phase gives a
``RequestedSpeciesNotRegisteredInPhase`` error.

JPL-19 Ternary Chemical Activation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A new ``TERNARY_CHEMICAL_ACTIVATION_JPL19`` reaction type uses the ternary chemical activation
equation from JPL Evaluation 19 (see :doc:`v1/reactions/ternary_chemical_activation_jpl19`). Its rate
constant is the sum of a Troe term and a chemical activation term. Each of the :math:`k_0`,
:math:`k_{\infty}`, and :math:`k_{int}` rate constants has its own reference temperature, with a
default of 298 K. The existing ``TERNARY_CHEMICAL_ACTIVATION`` type does not change.

- The new type requires version ``1.4.0`` or newer. A configuration with an older ``1.x`` version
  that uses it gives an ``InvalidVersion`` error.
- The newest supported version is now ``1.4``.
- ``types::TernaryChemicalActivationJPL19`` and ``Reactions::ternary_chemical_activation_jpl19`` hold
  the parsed reactions, and ``Validate()`` checks them.
- ``examples/v1/1.4`` has an example that uses the new type.

Version Checks
^^^^^^^^^^^^^^

The ``v1`` parser now checks the minor version of a configuration (see :ref:`v1-versions`):

- The ``"aerosol representations"`` and ``"aerosol processes"`` sections require version ``1.2.0`` or newer.
- The ``emissions`` section requires version ``1.3.0`` or newer.
- A minor version newer than the newest supported version gives an error.

A configuration that does not obey these rules gives an ``InvalidVersion`` error. Before this
change, the parser accepted these sections in any ``1.x`` version. To fix an older configuration,
set its version to ``1.2.0`` (aerosol) or ``1.3.0`` (emissions).

Other Changes
^^^^^^^^^^^^^

- The ``examples/v1`` directory has a complete example for each minor version.
- The documentation has new pages for the aerosol and emissions sections, an API reference,
  a getting started guide, and a contributors guide.

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

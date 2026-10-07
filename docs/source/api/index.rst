#############
API Reference
#############

These are the public C++ types and functions of the library. Include
``<mechanism_configuration/mechanism_configuration.hpp>`` to use all of them.

Parse
=====

.. doxygenfunction:: mechanism_configuration::Parse

.. doxygenfunction:: mechanism_configuration::ParseFromString

Validate
========

.. doxygenfunction:: mechanism_configuration::Validate

.. doxygenfunction:: mechanism_configuration::ValidateGasModel

.. doxygenfunction:: mechanism_configuration::ValidateAerosolModel

.. doxygenfunction:: mechanism_configuration::ValidateEmissionsModel

Mechanism
=========

.. doxygenstruct:: mechanism_configuration::Mechanism
   :members:

.. doxygenstruct:: mechanism_configuration::Version
   :members:

Errors
======

.. doxygentypedef:: mechanism_configuration::Errors

.. doxygenenum:: mechanism_configuration::ErrorCode

.. doxygenfunction:: mechanism_configuration::ErrorCodeToString

.. doxygenstruct:: mechanism_configuration::ErrorLocation
   :members:

Library version
===============

.. doxygenfunction:: mechanism_configuration::getVersionString

.. doxygenfunction:: mechanism_configuration::getVersionMajor

.. doxygenfunction:: mechanism_configuration::getVersionMinor

.. doxygenfunction:: mechanism_configuration::getVersionPatch

Types
=====

.. doxygennamespace:: mechanism_configuration::types
   :members:

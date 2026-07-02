################################################################################
# Preamble
cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# must be on the same line so that pyproject.toml can correctly identify the version
project(mechanism_configuration VERSION 2.0.0 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

message(STATUS "CMake build configuration for ${PROJECT_NAME} (${CMAKE_BUILD_TYPE}) ${PROJECT_VERSION}")

include(GNUInstallDirs)

################################################################################
# Projet wide setup options

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")

option(MECH_CONFIG_ENABLE_TESTS "Build the tests" ON)
option(MECH_CONFIG_BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(MECH_CONFIG_ENABLE_COVERAGE "Enable code coverage output" OFF)
option(MECH_CONFIG_USE_FMT "Use {fmt} library instead of std::format" OFF)
option(MECH_CONFIG_COMPILE_WARNING_AS_ERROR "Treat compiler warnings as errors for mechanism configuration targets" OFF)

set(MECH_CONFIG_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR})
set(MECH_CONFIG_LIB_DIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

################################################################################
# Dependencies

include(cmake/dependencies.cmake)

################################################################################
# Code coverage instrumentation
#
# These flags must be applied BEFORE add_subdirectory(src) so the library
# targets are compiled with coverage instrumentation. append_coverage_compiler_flags()
# only affects targets defined after it runs, so calling it later would leave the
# library uninstrumented and the report would cover almost nothing.

if(PROJECT_IS_TOP_LEVEL AND MECH_CONFIG_ENABLE_TESTS AND MECH_CONFIG_ENABLE_COVERAGE)
  include(cmake/CodeCoverage.cmake)
  append_coverage_compiler_flags()
endif()

################################################################################
# project source code

add_subdirectory(src)

################################################################################
# Tests

if(PROJECT_IS_TOP_LEVEL AND MECH_CONFIG_ENABLE_TESTS)
  # Test code coverage
  if(MECH_CONFIG_ENABLE_COVERAGE)
    setup_target_for_coverage_lcov(
        NAME coverage
        EXECUTABLE "ctest" -j 4
        EXCLUDE
            "${PROJECT_SOURCE_DIR}/test/*"
            "${CMAKE_BINARY_DIR}/_deps/*"
            "/usr/*"
        BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src"
        LCOV_ARGS --ignore-errors mismatch)
  endif()

  enable_testing()
  add_subdirectory(test)

  # Copy example folder to use as test data
  add_custom_target(copy_example_configs ALL ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/examples ${CMAKE_BINARY_DIR}/examples)

endif()

################################################################################
# Packaging

# only include packaging if we are the top level project being built
if(PROJECT_IS_TOP_LEVEL)
  add_subdirectory(packaging)
endif()
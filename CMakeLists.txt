################################################################################
# Preamble
cmake_minimum_required(VERSION 3.21)

# must be on the same line so that pyproject.toml can correctly identify the version
project(mechanism_configuration VERSION 0.2.0 LANGUAGES CXX)

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
option(MECH_CONFIG_ENABLE_PYTHON_LIBRARY "Build the python library" ON)
option(MECH_CONFIG_ENABLE_PIC "Build the library with position independent code" ON)
option(MECH_CONFIG_ENABLE_COVERAGE "Enable code coverage output" OFF)

set(MECH_CONFIG_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR})
set(MECH_CONFIG_LIB_DIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

################################################################################
# Dependencies

include(cmake/dependencies.cmake)

################################################################################
# project source code

add_subdirectory(src)

################################################################################
# python
if(MECH_CONFIG_ENABLE_PYTHON_LIBRARY)
  add_subdirectory(mechanism_configuration)
endif()

################################################################################
# Tests

if(PROJECT_IS_TOP_LEVEL AND MECH_CONFIG_ENABLE_TESTS)
  # Test code coverage
  if(MECH_CONFIG_ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    setup_target_for_coverage_lcov(
        NAME coverage
        EXECUTABLE "ctest"
        EXCLUDE "${PROJECT_SOURCE_DIR}/test/*"
        BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
    
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
if(PROJECT_IS_TOP_LEVEL AND NOT MECH_CONFIG_ENABLE_PYTHON_LIBRARY)
  add_subdirectory(packaging)
endif()
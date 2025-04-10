# Parse out a few individual variables
string(TIMESTAMP BUILD_CONFIG_DATE "%Y-%m-%d %H:%M:%S")
get_filename_component(C_COMPILER_NAME ${CMAKE_C_COMPILER} NAME)
set(C_COMPILER_NAME_INFO "${C_COMPILER_NAME} (${CMAKE_C_COMPILER_VERSION})")

if(MECH_CONFIG_ENABLE_PYTHON_LIBRARY)
  set(MECH_CONFIG_ENABLE_PYTHON_LIBRARY_INFO "ON (repo: ${PYBIND11_GIT_REPOSITORY} tag: ${PYBIND11_GIT_TAG})")
else()
  set(MECH_CONFIG_ENABLE_PYTHON_LIBRARY_INFO "OFF")
endif()

# Configure musica.settings file
configure_file("${PROJECT_SOURCE_DIR}/cmake/mechanism_configuration.settings.in"
  "${PROJECT_BINARY_DIR}/mechanism_configuration.settings"
  @ONLY)

# Read in settings file, print out.
# Avoid using system-specific calls so that this
# might also work on Windows.
file(READ "${PROJECT_BINARY_DIR}/mechanism_configuration.settings"
  MECHANISM_CONFIGURATION_SETTINGS)
message(STATUS ${MECHANISM_CONFIGURATION_SETTINGS})
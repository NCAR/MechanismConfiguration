################################################################################
# version
configure_file(version.hpp.in ${PROJECT_SOURCE_DIR}/include/mechanism_configuration/version.hpp @ONLY)

if(MECH_CONFIG_SHARED_LIBS)
  add_library(mechanism_configuration SHARED)
else()
  add_library(mechanism_configuration STATIC)
endif()

add_library(musica::mechanism_configuration ALIAS mechanism_configuration)

target_compile_features(mechanism_configuration PUBLIC cxx_std_20)

set_target_properties(mechanism_configuration PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${MECH_CONFIG_LIB_DIR}
  LIBRARY_OUTPUT_DIRECTORY ${MECH_CONFIG_LIB_DIR}
  RUNTIME_OUTPUT_DIRECTORY ${MECH_CONFIG_LIB_DIR}
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_sources(mechanism_configuration
  PRIVATE
    parse_status.cpp
    parser.cpp
    validate_schema.cpp
)

target_include_directories(mechanism_configuration
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(v0)
add_subdirectory(v1)
add_subdirectory(development)

target_link_libraries(mechanism_configuration 
  PUBLIC 
    yaml-cpp::yaml-cpp
)

if (APPLE)
  # set the rpath for the shared library
  set_target_properties(mechanism_configuration PROPERTIES
    INSTALL_RPATH "@loader_path"
    BUILD_WITH_INSTALL_RPATH TRUE
  )
elseif(UNIX)
  # set the rpath for the shared library
  set_target_properties(mechanism_configuration PROPERTIES
    INSTALL_RPATH "$ORIGIN"
    BUILD_WITH_INSTALL_RPATH TRUE
  )
endif()
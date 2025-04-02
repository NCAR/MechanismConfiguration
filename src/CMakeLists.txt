################################################################################
# version
configure_file(version.hpp.in ${PROJECT_SOURCE_DIR}/include/mechanism_configuration/version.hpp @ONLY)

if(OPEN_ATMOS_BUILD_SHARED_LIBS OR OPEN_ATMOS_ENABLE_PYTHON_LIBRARY)
  set(OPEN_ATMOS_LIBRARY_TYPE SHARED)
else()
  set(OPEN_ATMOS_LIBRARY_TYPE STATIC)
endif()

add_library(mechanism_configuration ${OPEN_ATMOS_LIBRARY_TYPE})
add_library(open_atmos::mechanism_configuration ALIAS mechanism_configuration)

target_compile_features(mechanism_configuration PUBLIC cxx_std_20)

if(OPEN_ATMOS_BUILD_SHARED_LIBS OR OPEN_ATMOS_ENABLE_PYTHON_LIBRARY)
  # Add the -fPIC flag for position-independent code if we are building shared libraries
  # or the python library so that it links correctly on linux
  set_target_properties(mechanism_configuration PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

set_target_properties(mechanism_configuration PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${OPEN_ATMOS_LIB_DIR}
  LIBRARY_OUTPUT_DIRECTORY ${OPEN_ATMOS_LIB_DIR}
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_sources(mechanism_configuration
  PRIVATE
    parse_status.cpp
    validate_schema.cpp
)

target_include_directories(mechanism_configuration
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(v0)
add_subdirectory(v1)

target_link_libraries(mechanism_configuration 
  PUBLIC 
    yaml-cpp::yaml-cpp
)

if (APPLE)
  # set the rpath for the shared library
  set_target_properties(mechanism_configuration PROPERTIES
    INSTALL_RPATH "@loader_path"
  )
elseif(UNIX)
  # set the rpath for the shared library
  set_target_properties(mechanism_configuration PROPERTIES
    INSTALL_RPATH "$ORIGIN"
    BUILD_WITH_INSTALL_RPATH TRUE
  )
endif()
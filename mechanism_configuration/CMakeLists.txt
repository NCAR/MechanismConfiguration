pybind11_add_module(_mechanism_configuration 
  wrapper.cpp
)

target_link_libraries(_mechanism_configuration PRIVATE mechanism_configuration)

# Set the rpath for the shared library
if(APPLE)
  message(STATUS "Building for MacOS")
  set_target_properties(_mechanism_configuration PROPERTIES
    INSTALL_RPATH "@loader_path"
    BUILD_WITH_INSTALL_RPATH TRUE
  )
elseif(UNIX)
  message(STATUS "Building for Linux")
  set_target_properties(_mechanism_configuration PROPERTIES
    INSTALL_RPATH "$ORIGIN"
    BUILD_WITH_INSTALL_RPATH TRUE
  )
endif()

if(WIN32)
  # makefiles on windows don't need the config directory
  if (${CMAKE_GENERATOR} MATCHES "MinGW Makefiles")
      set(PYTHON_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
  else()
  # but visual studio does
      set(PYTHON_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>")
  endif()
else()
  set(PYTHON_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
endif()

install(TARGETS _mechanism_configuration mechanism_configuration LIBRARY DESTINATION .)

pybind11_add_module(mechanism_configuration_python 
  wrapper.cpp
)

target_link_libraries(mechanism_configuration_python PRIVATE mechanism_configuration)
set_target_properties(mechanism_configuration_python PROPERTIES OUTPUT_NAME mechanism_configuration)

# Set the rpath for the shared library
if(APPLE)
  message(STATUS "Building for MacOS")
  set_target_properties(mechanism_configuration_python PROPERTIES
    INSTALL_RPATH "@loader_path"
    BUILD_WITH_INSTALL_RPATH TRUE
  )
elseif(UNIX)
  message(STATUS "Building for Linux")
  set_target_properties(mechanism_configuration_python PROPERTIES
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

install(TARGETS mechanism_configuration_python mechanism_configuration LIBRARY DESTINATION .)
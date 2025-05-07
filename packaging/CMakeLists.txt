include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
set(INSTALL_PREFIX "mechanism_configuration-${PROJECT_VERSION}" )

install(
  TARGETS
    mechanism_configuration
    yaml-cpp
  EXPORT 
    mechanism_configuration_Exports
  LIBRARY DESTINATION ${INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}
  PUBLIC_HEADER DESTINATION ${INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  DIRECTORY
    ${PROJECT_SOURCE_DIR}/include
  DESTINATION
    ${INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}
)

# install the cmake config files
set(cmake_config_install_location ${INSTALL_PREFIX}/cmake)

install(
  EXPORT 
    mechanism_configuration_Exports
  DESTINATION 
    ${cmake_config_install_location}
  NAMESPACE musica::
)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/mechanism_configurationConfig.cmake.in"
  "${PROJECT_BINARY_DIR}/mechanism_configurationConfig.cmake"
  INSTALL_DESTINATION
    ${cmake_config_install_location}
)

write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/mechanism_configurationConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  FILES
    ${PROJECT_BINARY_DIR}/mechanism_configurationConfig.cmake
    ${PROJECT_BINARY_DIR}/mechanism_configurationConfigVersion.cmake
  DESTINATION
    ${cmake_config_install_location}
)

######################################################################
# uninstall target

# https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#can-i-do-make-uninstall-with-cmake
if(NOT TARGET uninstall)
  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
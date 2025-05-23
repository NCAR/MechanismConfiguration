################################################################################
# build and add a standard test

function(create_standard_test)
  set(prefix TEST)
  set(optionalValues SKIP_MEMCHECK)
  set(singleValues NAME WORKING_DIRECTORY)
  set(multiValues SOURCES LIBRARIES)

  include(CMakeParseArguments)
  cmake_parse_arguments(${prefix} "${optionalValues}" "${singleValues}" "${multiValues}" ${ARGN})

  add_executable(test_${TEST_NAME} ${TEST_SOURCES})

  target_link_libraries(test_${TEST_NAME} PUBLIC musica::mechanism_configuration GTest::gtest_main)


  # link additional libraries
  foreach(library ${TEST_LIBRARIES})
    target_link_libraries(test_${TEST_NAME} PUBLIC ${library})
  endforeach()

  if(NOT DEFINED TEST_WORKING_DIRECTORY)
    set(TEST_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
  endif()

  add_mech_config_test(${TEST_NAME} test_${TEST_NAME} "" ${TEST_WORKING_DIRECTORY})
endfunction(create_standard_test)

################################################################################
# Add a test

function(add_mech_config_test test_name test_binary test_args working_dir)
  add_test(NAME ${test_name}
            COMMAND ${test_binary} ${test_args}
            WORKING_DIRECTORY ${working_dir})
endfunction(add_mech_config_test)

################################################################################

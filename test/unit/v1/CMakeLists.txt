################################################################################
# Tests

create_standard_test(NAME v1_parse_phases SOURCES test_parse_phases.cpp)
create_standard_test(NAME v1_parse_species SOURCES test_parse_species.cpp)

################################################################################
# Copy test data

add_custom_target(copy_v1_unit_test_configs ALL ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/v1_unit_configs ${CMAKE_BINARY_DIR}/v1_unit_configs)

################################################################################

add_subdirectory(models)
add_subdirectory(reactions)
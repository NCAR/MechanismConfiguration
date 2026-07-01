################################################################################
# Tests

create_standard_test(NAME emissions_v1_parse SOURCES test_parse_emissions.cpp)

################################################################################
# Copy test fixtures

add_custom_target(copy_emissions_unit_test_configs ALL
  ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/emissions_unit_configs
  ${CMAKE_BINARY_DIR}/emissions_unit_configs)

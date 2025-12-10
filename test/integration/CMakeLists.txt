################################################################################
# Copy test data

add_custom_target(copy_integration_test_configs ALL ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/integration_configs ${CMAKE_BINARY_DIR}/integration_configs)

################################################################################
# Tests
create_standard_test(NAME parser SOURCES test_parser.cpp)
create_standard_test(NAME v0_parser SOURCES test_v0_parser.cpp)
create_standard_test(NAME v1_parser SOURCES test_v1_parser.cpp)
create_standard_test(NAME development_parser SOURCES test_development_parser.cpp)

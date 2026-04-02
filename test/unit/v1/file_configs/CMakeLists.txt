################################################################################
# Tests

create_standard_test(NAME v1_parse_from_file_configs SOURCES test_parse_from_file_configs.cpp)

################################################################################
# Copy test config data

add_custom_target(copy_v1_file_configs ALL ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/configs ${CMAKE_BINARY_DIR}/test/unit/v1/file_configs/configs)

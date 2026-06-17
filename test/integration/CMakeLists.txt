################################################################################
# Copy test data

add_custom_target(copy_integration_test_configs ALL ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/integration_configs ${CMAKE_BINARY_DIR}/integration_configs)

################################################################################
# Tests
create_standard_test(NAME parser SOURCES test_parser.cpp)
create_standard_test(NAME v0_parser SOURCES test_v0_parser.cpp)
create_standard_test(NAME v1_parser SOURCES test_v1_parser.cpp)
create_standard_test(NAME v1_read_from_file_configs SOURCES test_v1_read_from_file_configs.cpp)

################################################################################
# README integration test
# Extract the first ```cpp ... ``` code block from README.md at configure time
# and compile it as a standalone integration test.

file(READ "${PROJECT_SOURCE_DIR}/README.md" _readme_content)

string(FIND "${_readme_content}" "```cpp\n" _fence_start)
if(_fence_start EQUAL -1)
  message(FATAL_ERROR "README integration test: could not find opening ```cpp fence in README.md")
endif()
math(EXPR _code_start "${_fence_start} + 7") # skip "```cpp\n" (7 chars)
string(SUBSTRING "${_readme_content}" ${_code_start} -1 _after_fence)

string(FIND "${_after_fence}" "\n```" _fence_end)
if(_fence_end EQUAL -1)
  message(FATAL_ERROR "README integration test: could not find closing ``` fence in README.md")
endif()
string(SUBSTRING "${_after_fence}" 0 ${_fence_end} _readme_example_code)

set(_readme_example_dir "${CMAKE_BINARY_DIR}/readme_example")
set(_readme_example_src "${_readme_example_dir}/main.cpp")
file(MAKE_DIRECTORY "${_readme_example_dir}")
file(WRITE "${_readme_example_src}" "${_readme_example_code}")

add_executable(test_readme_example "${_readme_example_src}")
target_link_libraries(test_readme_example PUBLIC musica::mechanism_configuration)
add_mech_config_test(readme_example test_readme_example "" "${CMAKE_BINARY_DIR}")

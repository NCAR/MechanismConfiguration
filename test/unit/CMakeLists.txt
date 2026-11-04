create_standard_test(NAME format_compat SOURCES test_format_compat.cpp)
create_standard_test(NAME validate SOURCES test_validate.cpp)

add_subdirectory(v0)
add_subdirectory(v1)
add_subdirectory(emissions)

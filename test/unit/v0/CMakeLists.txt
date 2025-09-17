################################################################################
# Tests

create_standard_test(NAME v0_parse_arrhenius SOURCES test_arrhenius_config.cpp)
create_standard_test(NAME v0_parse_branched SOURCES test_branched_config.cpp)
create_standard_test(NAME v0_parse_emission SOURCES test_emission_config.cpp)
create_standard_test(NAME v0_parse_first_order_loss SOURCES test_first_order_loss_config.cpp)
create_standard_test(NAME v0_parse_photolysis SOURCES test_photolysis_config.cpp)
create_standard_test(NAME v0_parse_surface SOURCES test_surface_config.cpp)
create_standard_test(NAME v0_parse_ternary_chemical_activation SOURCES test_ternary_chemical_activation_config.cpp)
create_standard_test(NAME v0_parse_troe SOURCES test_troe_config.cpp)
create_standard_test(NAME v0_parse_tunneling SOURCES test_tunneling_config.cpp)
create_standard_test(NAME v0_parse_user_defined SOURCES test_user_defined_config.cpp)
create_standard_test(NAME v0_parse_species SOURCES test_species_config.cpp)

################################################################################
# Copy test data

add_custom_target(copy_v0_unit_test_configs ALL ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/v0_unit_configs ${CMAKE_BINARY_DIR}/v0_unit_configs)

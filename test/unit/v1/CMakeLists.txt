################################################################################
# Test utilities

include(test_util)

################################################################################
# Tests

create_standard_test(NAME v1_parse_arrhenius SOURCES test_parse_arrhenius.cpp)
create_standard_test(NAME v1_parse_branched SOURCES test_parse_branched.cpp)
create_standard_test(NAME v1_parse_condensed_phase_arrhenius SOURCES test_parse_condensed_phase_arrhenius.cpp)
create_standard_test(NAME v1_parse_condensed_phase_photolysis SOURCES test_parse_condensed_phase_photolysis.cpp)
create_standard_test(NAME v1_parse_emission SOURCES test_parse_emission.cpp)
create_standard_test(NAME v1_parse_first_order_loss SOURCES test_parse_first_order_loss.cpp)
create_standard_test(NAME v1_parse_henrys_law SOURCES test_parse_henrys_law.cpp)
create_standard_test(NAME v1_parse_phases SOURCES test_parse_phases.cpp)
create_standard_test(NAME v1_parse_photolysis SOURCES test_parse_photolysis.cpp)
create_standard_test(NAME v1_parse_species SOURCES test_parse_species.cpp)
create_standard_test(NAME v1_parse_surface SOURCES test_parse_surface.cpp)
create_standard_test(NAME v1_parse_troe SOURCES test_parse_troe.cpp)
create_standard_test(NAME v1_parse_tunneling SOURCES test_parse_tunneling.cpp)
create_standard_test(NAME v1_parse_simpol_phase_transfer SOURCES test_parse_simpol_phase_transfer.cpp)
create_standard_test(NAME v1_parse_aqueous_equilibrium SOURCES test_parse_aqueous_equilibrium.cpp)
create_standard_test(NAME v1_parse_wet_deposition SOURCES test_parse_wet_deposition.cpp)
create_standard_test(NAME v1_parse_user_defined SOURCES test_parse_user_defined.cpp)

################################################################################
# Copy test data

add_custom_target(copy_v1_unit_test_configs ALL ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/v1_unit_configs ${CMAKE_BINARY_DIR}/v1_unit_configs)
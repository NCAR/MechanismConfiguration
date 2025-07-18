# CMake generated Testfile for 
# Source directory: /home/runner/work/MechanismConfiguration/MechanismConfiguration/test/unit/v1
# Build directory: /home/runner/work/MechanismConfiguration/MechanismConfiguration/build_test/test/unit/v1
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[v1_parse_phases]=] "/home/runner/work/MechanismConfiguration/MechanismConfiguration/build_test/test_v1_parse_phases")
set_tests_properties([=[v1_parse_phases]=] PROPERTIES  WORKING_DIRECTORY "/home/runner/work/MechanismConfiguration/MechanismConfiguration/build_test" _BACKTRACE_TRIPLES "/home/runner/work/MechanismConfiguration/MechanismConfiguration/cmake/test_util.cmake;34;add_test;/home/runner/work/MechanismConfiguration/MechanismConfiguration/cmake/test_util.cmake;27;add_mech_config_test;/home/runner/work/MechanismConfiguration/MechanismConfiguration/test/unit/v1/CMakeLists.txt;4;create_standard_test;/home/runner/work/MechanismConfiguration/MechanismConfiguration/test/unit/v1/CMakeLists.txt;0;")
add_test([=[v1_parse_species]=] "/home/runner/work/MechanismConfiguration/MechanismConfiguration/build_test/test_v1_parse_species")
set_tests_properties([=[v1_parse_species]=] PROPERTIES  WORKING_DIRECTORY "/home/runner/work/MechanismConfiguration/MechanismConfiguration/build_test" _BACKTRACE_TRIPLES "/home/runner/work/MechanismConfiguration/MechanismConfiguration/cmake/test_util.cmake;34;add_test;/home/runner/work/MechanismConfiguration/MechanismConfiguration/cmake/test_util.cmake;27;add_mech_config_test;/home/runner/work/MechanismConfiguration/MechanismConfiguration/test/unit/v1/CMakeLists.txt;5;create_standard_test;/home/runner/work/MechanismConfiguration/MechanismConfiguration/test/unit/v1/CMakeLists.txt;0;")
subdirs("models")
subdirs("reactions")

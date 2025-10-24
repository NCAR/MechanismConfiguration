target_sources(mechanism_configuration
  PRIVATE
    aqueous_equilibrium_parser.cpp
    condensed_phase_photolysis_parser.cpp
    henrys_law_parser.cpp
    simpol_phase_transfer_parser.cpp
    wet_deposition_parser.cpp
)

add_subdirectory(parsers)
add_subdirectory(validators)
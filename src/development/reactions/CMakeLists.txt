target_sources(mechanism_configuration
  PRIVATE
    aqueous_equilibrium_parser.cpp
    # arrhenius_parser.cpp
    branched_parser.cpp
    condensed_phase_arrhenius_parser.cpp
    condensed_phase_photolysis_parser.cpp
    emission_parser.cpp
    first_order_loss_parser.cpp
    henrys_law_parser.cpp
    photolysis_parser.cpp
    simpol_phase_transfer_parser.cpp
    surface_parser.cpp
    taylor_series_parser.cpp
    troe_parser.cpp
    ternary_chemical_activation_parser.cpp
    tunneling_parser.cpp
    wet_deposition_parser.cpp
    user_defined_parser.cpp
)

add_subdirectory(parsers)
add_subdirectory(validators)
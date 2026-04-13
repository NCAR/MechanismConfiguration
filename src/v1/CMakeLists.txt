target_sources(mechanism_configuration
  PRIVATE
    parser.cpp
    mechanism_parsers.cpp
    aerosol_parsers.cpp
    utils.cpp
)

add_subdirectory(reactions)
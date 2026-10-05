target_sources(mechanism_configuration
  PRIVATE
    parser.cpp
    utils.cpp
)

add_subdirectory(aerosol)
add_subdirectory(emissions)
add_subdirectory(reactions)
add_subdirectory(species)

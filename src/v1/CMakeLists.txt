target_sources(mechanism_configuration
  PRIVATE
    parser.cpp
    type_parsers.cpp
    type_validators.cpp
    utils.cpp
)

add_subdirectory(reactions)
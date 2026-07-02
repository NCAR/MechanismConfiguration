target_sources(mechanism_configuration
  PRIVATE
    emissions_parser.cpp
    parser.cpp
    type_parsers.cpp
    type_schema.cpp
    utils.cpp
)

add_subdirectory(reactions)
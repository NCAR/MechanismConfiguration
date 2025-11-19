target_sources(mechanism_configuration
  PRIVATE
    mechanism_parser.cpp
    type_parsers.cpp
    type_validator.cpp
    utils.cpp
)

add_subdirectory(models)
add_subdirectory(reactions)
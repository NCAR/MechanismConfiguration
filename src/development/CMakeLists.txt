target_sources(mechanism_configuration
  PRIVATE
    parser.cpp
    mechanism_parsers.cpp
    utils.cpp
    validator.cpp
)

add_subdirectory(models)
add_subdirectory(reactions)
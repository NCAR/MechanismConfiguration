target_sources(mechanism_configuration
  PRIVATE
    compatability.cpp
    mechanism.cpp
    type_parsers.cpp
    type_validators.cpp
    utils.cpp
)

add_subdirectory(models)
add_subdirectory(reactions)
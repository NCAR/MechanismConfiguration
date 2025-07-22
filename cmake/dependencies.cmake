include(FetchContent)

################################################################################
# Function to reduce repeated code, set a value to a variable only if the
# variable is not already defined. 
function(set_git_default git_var git_val)

  if(NOT ${git_var})
    set(${git_var} ${git_val} PARENT_SCOPE)
  endif()

endfunction(set_git_default)

################################################################################
# google test

if(PROJECT_IS_TOP_LEVEL AND MECH_CONFIG_ENABLE_TESTS)
  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    FIND_PACKAGE_ARGS NAMES GTest
  )

  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(googletest)
endif()

################################################################################
# yaml-cpp

set_git_default(YAML_CPP_GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git)
set_git_default(YAML_CPP_GIT_TAG 28f93bdec6387d42332220afa9558060c8016795)

FetchContent_Declare(yaml-cpp
    GIT_REPOSITORY ${YAML_CPP_GIT_REPOSITORY}
    GIT_TAG        ${YAML_CPP_GIT_TAG}
    FIND_PACKAGE_ARGS NAMES yaml-cpp
)

set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(yaml-cpp)
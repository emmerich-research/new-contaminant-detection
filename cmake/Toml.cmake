# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

CPMAddPackage(
  NAME toml
  GITHUB_REPOSITORY ToruNiina/toml11
  GIT_TAG 38e113d2dc898ead67b2135cd9affdab87fbf2df
  OPTIONS "CMAKE_CXX_STANDARD 20" "toml11_BUILD_TEST OFF"
)

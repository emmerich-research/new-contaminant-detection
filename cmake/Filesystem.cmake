# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

CPMAddPackage(
  NAME filesystem
  GITHUB_REPOSITORY gulrak/filesystem
  VERSION 1.3.4
)

if(filesystem_ADDED)
  add_library(fs::fs ALIAS ghc_filesystem)
endif()

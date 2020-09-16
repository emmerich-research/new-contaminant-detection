# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

CPMAddPackage(
  NAME cotire
  GITHUB_REPOSITORY sakra/cotire
  GIT_TAG 9d8e3f7b584b934edfe2410d87728137a00e4d8b
  DOWNLOAD_ONLY True
)

include(${cotire_SOURCE_DIR}/CMake/cotire.cmake)

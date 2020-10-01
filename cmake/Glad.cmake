# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

# set(GLAD_API "" CACHE STRING "API type/version pairs, like \"gl=3.2,gles=\", no version means
# latest" FORCE ) set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE )
# add_subdirectory(${PROJECT_SOURCE_DIR}/external/glad EXCLUDE_FROM_ALL)

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

CPMAddPackage(
  NAME glad
  GITHUB_REPOSITORY Dav1dde/glad
  GIT_TAG a5ca31c88a4cc5847ea012629aff3690f261c7c4
)

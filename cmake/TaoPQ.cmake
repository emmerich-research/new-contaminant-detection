# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

CPMAddPackage(
  NAME taopq
  GITHUB_REPOSITORY taocpp/taopq
  GIT_TAG 8cbbe7b04ef772c340754054e928636865c88029
  OPTIONS "TAOPQ_BUILD_TESTS OFF"
)

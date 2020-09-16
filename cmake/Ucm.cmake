# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Cotire.cmake)

CPMAddPackage(
  NAME ucm
  GITHUB_REPOSITORY onqtam/ucm
  GIT_TAG 04a11317552738f71cee473eac8f4ff861740ce3
  DOWNLOAD_ONLY True
)

include(${ucm_SOURCE_DIR}/cmake/ucm.cmake)

if(ucm_ADDED)
  # enable unity build
  set(UCM_UNITY_BUILD ON)
endif()

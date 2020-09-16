# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

CPMAddPackage(
  NAME spdlog
  GITHUB_REPOSITORY gabime/spdlog
  VERSION 1.6.1
  OPTIONS "SPDLOG_FMT_EXTERNAL OFF" "SPDLOG_FMT_EXTERNAL_HO ON" "SPDLOG_BUILD_SHARED OFF"
          "SPDLOG_ENABLE_PCH ON"
)

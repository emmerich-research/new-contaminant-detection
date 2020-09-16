# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

CPMAddPackage(
  NAME modbuscpp
  GITHUB_REPOSITORY rayandrews/modbus-cpp
  VERSION 1.0.0
)

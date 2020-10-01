# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

CPMAddPackage(
  NAME modbuscpp
  GITHUB_REPOSITORY rayandrews/modbus-cpp
  GIT_TAG 84d8b5d3a551035783c04f8efe86b4aadb8faaa9
)

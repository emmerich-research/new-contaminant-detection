# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

CPMAddPackage(
  NAME modbuscpp
  GITHUB_REPOSITORY rayandrews/modbus-cpp
  GIT_TAG 3492ba1b70fb5511db102f4bcad77aadb249149f
)

# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

CPMAddPackage(
  NAME modbuscpp
  GITHUB_REPOSITORY rayandrews/modbus-cpp
  GIT_TAG b8295c421bc97fd0f52c705796da06922d625ad5
)

# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

find_package(SQLite3 REQUIRED)

CPMAddPackage(
  NAME sqlite_orm
  GITHUB_REPOSITORY fnc12/sqlite_orm
  VERSION 1.5
  OPTIONS "SQLITE_ORM_ENABLE_CXX_17 ON" "SQLITE_ORM_ENABLE_CXX_17 OFF"
)

if(SQLite3_FOUND AND sqlite_orm_ADDED)
  set(SQLite_FOUND ON)
endif()

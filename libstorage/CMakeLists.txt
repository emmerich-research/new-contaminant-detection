project(storage)

ucm_add_files(
  "internal-database.cpp"
  "listener.cpp"

  TO SOURCES)

ucm_add_target(
  NAME storage
  TYPE STATIC
  SOURCES ${SOURCES}
  UNITY CPP_PER_UNITY 20
  PCH_FILE "storage.hpp")

add_library("${PROJECT_NAMESPACE}::storage" ALIAS storage)

target_include_directories(storage PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
  "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>"
  "$<BUILD_INTERFACE:${SQLite3_INCLUDE_DIRS}>"
  "$<BUILD_INTERFACE:${SqliteOrm_INCLUDE}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(storage PUBLIC
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  ${SQLite3_LIBRARIES}
  ${OpenCV_LIBS}
  sqlite_orm
  "${PROJECT_NAMESPACE}::util"
  "${PROJECT_NAMESPACE}::core"
  "${PROJECT_NAMESPACE}::gui"
  "${PROJECT_NAMESPACE}::networking")

target_set_warnings(storage
  ENABLE ALL
  DISABLE Annoying)

set_target_properties(storage PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

target_enable_lto(storage optimized)

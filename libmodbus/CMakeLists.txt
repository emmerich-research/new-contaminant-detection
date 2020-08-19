project(modbus)

ucm_add_files(
  "modbus-data-table.cpp"

  "modbus-bit-read.cpp"

  "modbus-connection.cpp"
  "modbus-connection-manager.cpp"

  "modbus-server.cpp"

  TO SOURCES)

ucm_add_target(
  NAME modbus
  TYPE STATIC
  SOURCES ${SOURCES}
  UNITY CPP_PER_UNITY 20
  PCH_FILE "modbus.hpp")

add_library("${PROJECT_NAMESPACE}::modbus" ALIAS modbus)

target_include_directories(modbus SYSTEM PRIVATE
  "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
  "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>"
  "$<BUILD_INTERFACE:${Asio2_INCLUDE_DIR}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(modbus PUBLIC
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  fmt::fmt)

target_set_warnings(modbus
  ENABLE ALL
  DISABLE Annoying)

set_target_properties(modbus PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

target_enable_lto(modbus optimized)

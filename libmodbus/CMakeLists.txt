project(modbus)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
  add_definitions(-DDEBUG_ON)
endif()

ucm_add_files(
  "modbus-logger.cpp"

  "modbus-data-table.cpp"

  "modbus-adu.cpp"
  "modbus-request.cpp"
  "modbus-response.cpp"

  "modbus-bit-read.cpp"
  "modbus-bit-write.cpp"

  "modbus-request-handler.cpp"
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
  fmt::fmt
  struc)

target_set_warnings(modbus
  ENABLE ALL
  DISABLE Annoying)

set_target_properties(modbus PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO)

target_compile_options(modbus PRIVATE
  -Wno-sign-conversion
  -Wno-sign-compare
  -Wno-zero-as-null-pointer-constant)

target_enable_lto(modbus optimized)

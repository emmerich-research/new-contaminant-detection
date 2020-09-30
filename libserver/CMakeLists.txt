project(server)

ucm_add_files(
  "logger.cpp"
  "config.cpp"
  "slave.cpp"
  "data-mapper.cpp"
  "data-window.cpp"
  TO
  SOURCES
)

ucm_add_target(
  NAME
  server
  TYPE
  STATIC
  SOURCES
  ${SOURCES}
  UNITY
  CPP_PER_UNITY
  20
  PCH_FILE
  "server.hpp"
)

add_library("${PROJECT_NAMESPACE}::server" ALIAS server)

target_include_directories(
  server
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(
  server
  PUBLIC ${CMAKE_THREAD_LIBS_INIT}
         asio2
         struc
         fmt
         modbuscpp
         "${PROJECT_NAMESPACE}::util"
         "${PROJECT_NAMESPACE}::core"
         "${PROJECT_NAMESPACE}::gui"
)

target_set_warnings(server ENABLE ALL DISABLE Annoying)

set_target_properties(
  server
  PROPERTIES CXX_STANDARD 20
             CXX_STANDARD_REQUIRED YES
             CXX_EXTENSIONS NO
)

target_enable_lto(server optimized)

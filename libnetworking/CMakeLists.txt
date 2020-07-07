project(networking)

ucm_add_files(
  "modbus.cpp"
  "modbus-tcp.cpp"

  TO SOURCES)
  
ucm_add_target(
  NAME networking
  TYPE STATIC
  SOURCES ${SOURCES}
  UNITY CPP_PER_UNITY 20
  PCH_FILE "networking.hpp")

add_library("${PROJECT_NAMESPACE}::networking" ALIAS networking)

target_include_directories(networking PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
  "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(networking PUBLIC
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  "${PROJECT_NAMESPACE}::util"
  "${PROJECT_NAMESPACE}::core")

target_set_warnings(networking
  ENABLE ALL
  DISABLE Annoying)

set_target_properties(networking PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES 
    CXX_EXTENSIONS NO)

target_enable_lto(networking optimized)

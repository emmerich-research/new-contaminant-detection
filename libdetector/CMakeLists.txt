project(detector)

ucm_add_files(
  "base.cpp"
  "blob.cpp"
  "util.cpp"

  TO SOURCES)

ucm_add_target(
  NAME detector
  TYPE STATIC
  SOURCES ${SOURCES}
  UNITY CPP_PER_UNITY 20
  PCH_FILE "detector.hpp")

add_library("${PROJECT_NAMESPACE}::detector" ALIAS detector)

target_include_directories(detector PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(detector PUBLIC
  ${OpenCV_LIBS}
  "${PROJECT_NAMESPACE}::util"
  "${PROJECT_NAMESPACE}::core")

target_set_warnings(detector
  ENABLE ALL
  DISABLE Annoying)

set_target_properties(detector PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

target_enable_lto(detector optimized)

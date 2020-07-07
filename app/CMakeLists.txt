project(app)

ucm_add_files(
  "main.cpp"
  TO SOURCES)
  
ucm_add_target(
  NAME app
  TYPE EXECUTABLE
  SOURCES ${SOURCES})

target_include_directories(app PUBLIC
  "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>")

target_link_libraries(app PRIVATE
  ${Boost_LIBRARIES}
  "${PROJECT_NAMESPACE}::util"
  "${PROJECT_NAMESPACE}::core"
  "${PROJECT_NAMESPACE}::networking")

target_set_warnings(app
  ENABLE ALL
  DISABLE Annoying)

set_target_properties(app PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES 
    CXX_EXTENSIONS NO)

target_enable_lto(app optimized)

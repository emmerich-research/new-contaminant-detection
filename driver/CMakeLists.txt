project(driver)

file(GLOB_RECURSE driver_sources "*.cpp")

foreach(driver_file ${driver_sources})
  # I used a simple string replace, to cut off .cpp.
  get_filename_component(driver_source_file ${driver_file} NAME)
  string(REPLACE ".cpp" "" driver_exe ${driver_source_file})

  add_executable(${driver_exe} ${driver_file})

  target_include_directories(${driver_exe} PRIVATE
    ${Boost_INCLUDE_DIR})

  target_link_libraries(${driver_exe} PRIVATE
    ${Boost_LIBRARIES}
    "${PROJECT_NAMESPACE}::util"
    "${PROJECT_NAMESPACE}::core")

  target_set_warnings(${driver_exe}
    ENABLE ALL
    DISABLE Annoying)

  set_target_properties(${driver_exe} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

  target_enable_lto(${driver_exe} optimized)
endforeach()

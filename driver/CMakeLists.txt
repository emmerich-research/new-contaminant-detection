project(driver)

file(GLOB_RECURSE driver_sources "*.cpp")

foreach(driver_file ${driver_sources})
  get_filename_component(driver_source_file ${driver_file} NAME)
  string(REPLACE ".cpp" "" driver_exe ${driver_source_file})

  add_executable(${driver_exe} ${driver_file})

  target_include_directories(${driver_exe} SYSTEM PRIVATE ${Boost_INCLUDE_DIR} ${Asio2_INCLUDE_DIR})

  target_link_libraries(
    ${driver_exe}
    PRIVATE ${CMAKE_THREAD_LIBS_INIT}
            ${Boost_LIBRARIES}
            asio2
            struc
            fmt
            modbuscpp
            "${PROJECT_NAMESPACE}::util"
            "${PROJECT_NAMESPACE}::core"
            "${PROJECT_NAMESPACE}::networking"
  )

  target_set_warnings(${driver_exe} ENABLE ALL DISABLE Annoying)

  set_target_properties(
    ${driver_exe}
    PROPERTIES CXX_STANDARD 20
               CXX_STANDARD_REQUIRED YES
               CXX_EXTENSIONS NO
  )

  target_compile_options(
    ${driver_exe} PRIVATE -Wno-sign-conversion -Wno-sign-compare -Wno-zero-as-null-pointer-constant
                          -Wno-deprecated-copy-dtor -Wno-weak-vtables
  )

  target_enable_lto(${driver_exe} optimized)
endforeach()

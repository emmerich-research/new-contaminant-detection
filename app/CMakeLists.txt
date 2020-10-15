project(app)

set(SOURCES "app.cpp" "database.cpp" "image.cpp")

foreach(app_src ${SOURCES})
  get_filename_component(app_src_file ${app_src} NAME)
  string(REPLACE ".cpp" "" app_exe ${app_src_file})

  ucm_add_target(NAME ${app_exe} TYPE EXECUTABLE SOURCES ${app_src})

  # target_include_directories(app PUBLIC)

  target_link_libraries(
    ${app_exe}
    PRIVATE "${PROJECT_NAMESPACE}::util"
            "${PROJECT_NAMESPACE}::core"
            "${PROJECT_NAMESPACE}::server"
            "${PROJECT_NAMESPACE}::detector"
            "${PROJECT_NAMESPACE}::gui"
            "${PROJECT_NAMESPACE}::storage"
            "${PROJECT_NAMESPACE}::cloud"
  )

  target_set_warnings(${app_exe} ENABLE ALL DISABLE Annoying)

  set_target_properties(
    ${app_exe}
    PROPERTIES CXX_STANDARD 20
               CXX_STANDARD_REQUIRED YES
               CXX_EXTENSIONS NO
  )

  target_enable_lto(${app_exe} optimized)
endforeach()

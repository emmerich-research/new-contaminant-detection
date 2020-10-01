project(app)

ucm_add_files("main.cpp" TO SOURCES)

ucm_add_target(NAME app TYPE EXECUTABLE SOURCES ${SOURCES})

# target_include_directories(app PUBLIC)

target_link_libraries(
  app
  PRIVATE "${PROJECT_NAMESPACE}::util" "${PROJECT_NAMESPACE}::core" "${PROJECT_NAMESPACE}::server"
          "${PROJECT_NAMESPACE}::detector" "${PROJECT_NAMESPACE}::gui"
          "${PROJECT_NAMESPACE}::storage"
)

target_set_warnings(app ENABLE ALL DISABLE Annoying)

set_target_properties(
  app
  PROPERTIES CXX_STANDARD 20
             CXX_STANDARD_REQUIRED YES
             CXX_EXTENSIONS NO
)

target_enable_lto(app optimized)

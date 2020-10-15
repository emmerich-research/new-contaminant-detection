project(cloud)

ucm_add_files("config.cpp" "database.cpp" "listener.cpp" "storage.cpp" TO SOURCES)

ucm_add_target(
  NAME
  cloud
  TYPE
  STATIC
  SOURCES
  ${SOURCES}
  UNITY
  CPP_PER_UNITY
  20
  PCH_FILE
  "cloud.hpp"
)

add_library("${PROJECT_NAMESPACE}::cloud" ALIAS cloud)

target_include_directories(
  cloud
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
         "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>" "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>"
         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(
  cloud
  PUBLIC ${CMAKE_THREAD_LIBS_INIT}
         ${Boost_LIBRARIES}
         "${PROJECT_NAMESPACE}::util"
         "${PROJECT_NAMESPACE}::core"
         "${PROJECT_NAMESPACE}::gui"
         "${PROJECT_NAMESPACE}::storage"
         taocpp::taopq
         ${GCLOUD_LIBS}
)

# google_cloud_cpp_add_common_options(cloud)

target_set_warnings(cloud ENABLE ALL DISABLE Annoying)

set_target_properties(
  cloud
  PROPERTIES CXX_STANDARD 20
             CXX_STANDARD_REQUIRED YES
             CXX_EXTENSIONS NO
)

target_enable_lto(cloud optimized)

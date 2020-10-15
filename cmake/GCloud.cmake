# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

find_package(storage_client REQUIRED)
find_package(google_cloud_cpp_grpc_utils REQUIRED)

set(GCLOUD_LIBS "storage_client" "google_cloud_cpp_common" ${c-ares_SHARED_LIBRARY})

# include(${CMAKE_CURRENT_LIST_DIR}/Abseil.cmake)

# CPMAddPackage( NAME gcloud-cpp GITHUB_REPOSITORY googleapis/google-cloud-cpp VERSION 1.18.0
# DOWNLOAD_ONLY ON # OPTIONS "GOOGLE_CLOUD_CPP_ENABLE_GRPC OFF" )

# if(gcloud-cpp_ADDED) add_subdirectory(${gcloud-cpp_SOURCE_DIR}/super)
# add_subdirectory(${gcloud-cpp_SOURCE_DIR}) endif()

# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

# OpenCV library
find_package(OpenCV REQUIRED core xfeatures2d videoio)
set(WITH_GTK
    OFF
    CACHE BOOL "" FORCE
)
set(WITH_QT
    OFF
    CACHE BOOL "" FORCE
)

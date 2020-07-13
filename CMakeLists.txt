cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)


# Definitions
set(PROJECT_TITLE           "contaminant-detection")
set(PROJECT_ALT_TITLE       "EmmerichContaminantDetection")
set(PROJECT_DESCRIPTION     "Emmerich Contaminant Detection")
set(PROJECT_AUTHOR          "emmerich")
set(PROJECT_NAMESPACE       ${PROJECT_AUTHOR})
set(PROJECT_VERSION_MAJOR   0)
set(PROJECT_VERSION_MINOR   0)
set(PROJECT_VERSION_PATCH   1)
set(PROJECT_VERSION         "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

project(${PROJECT_TITLE}
  VERSION ${PROJECT_VERSION}
  DESCRIPTION ${PROJECT_DESCRIPTION}
  LANGUAGES CXX)

option(ENABLE_WARNINGS_SETTINGS "Allow target_set_warnings to add flags and defines.
                                 Set this to OFF if you want to provide your own warning parameters." ON)
option(ENABLE_LTO "Enable link time optimization" ON)
option(ENABLE_DOCTESTS "Include tests in the library. Setting this to OFF will remove all doctest related code. Tests in tests/*.cpp will still be enabled." OFF)
option(BUILD_SHARED_LIBS "Build shared libs in order for some libraries to work" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include stuffs
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_SOURCE_DIR}/external/cotire/CMake/"
  "${CMAKE_SOURCE_DIR}/external/ucm/cmake/"
  "${CMAKE_SOURCE_DIR}/cmake/")

include(GNUInstallDirs)
include(ConfigSafeGuards)
include(Colors)
include(cotire)
include(CTest)
include(Doctest)
include(Documentation)
include(Filesystem)
include(Glad)
include(LTO)
include(Misc)
include(Spdlog)
include(Toml)
include(ucm)
include(Warnings)

# enable unity build
set(UCM_UNITY_BUILD ON)

# Check for LTO support.
find_lto(CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

enable_testing()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
  add_definitions(-DNDEBUG)
endif()

add_definitions(-DATM_COMPILATION)

find_package(Threads REQUIRED)

# Boost library
set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
find_package(Boost 1.71.0 REQUIRED COMPONENTS
  system)

add_definitions(-DBOOST_ASIO_DISABLE_CONCEPTS)

# OpenCV library
find_package(OpenCV REQUIRED
  core
  xfeatures2d
  videoio)
set(WITH_GTK OFF CACHE BOOL "" FORCE)
set(WITH_QT  OFF CACHE BOOL "" FORCE)

# OpenGL
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# IMGUI
set(IMGUI_VERSION 1.77)
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")
find_package(ImGui ${IMGUI_VERSION} REQUIRED)


if (Boost_FOUND)
  add_subdirectory(libutil)
  add_subdirectory(libcore)
  add_subdirectory(libnetworking)
  add_subdirectory(libdetector)
  add_subdirectory(libgui)
  add_subdirectory(driver)
  add_subdirectory(app)
endif()

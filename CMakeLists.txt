cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

# Definitions
set(PROJECT_TITLE "contaminant-detection")
set(PROJECT_ALT_TITLE "EmmerichContaminantDetection")
set(PROJECT_DESCRIPTION "Emmerich Contaminant Detection")
set(PROJECT_AUTHOR "emmerich")
set(PROJECT_NAMESPACE ${PROJECT_AUTHOR})
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 1)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

project(
  ${PROJECT_TITLE}
  VERSION ${PROJECT_VERSION}
  DESCRIPTION ${PROJECT_DESCRIPTION}
  LANGUAGES CXX
)

option(
  ENABLE_WARNINGS_SETTINGS
  "Allow target_set_warnings to add flags and defines.
                                 Set this to OFF if you want to provide your own warning parameters."
  ON
)
option(ENABLE_LTO "Enable link time optimization" ON)
option(
  ENABLE_DOCTESTS
  "Include tests in the library. Setting this to OFF will remove all doctest related code. Tests in tests/*.cpp will still be enabled."
  OFF
)
option(BUILD_SHARED_LIBS "Build shared libs in order for some libraries to work" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include stuffs
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(GNUInstallDirs)
include(ConfigSafeGuards)
include(Colors)
include(CTest)
include(Doctest)
include(Documentation)
include(LTO)
include(Misc)
include(Ucm)
include(Warnings)

find_package(Threads REQUIRED)
include(Asio2)
include(Boost)
include(Filesystem)
include(Imgui)
include(ModbusCPP)
include(OpenCV)
include(Toml)
include(Spdlog)
include(SqliteORM)

if(Boost_FOUND
   AND SQLite_FOUND
   AND OpenCV_FOUND
   AND IMGUI_FOUND
)
  # Check for LTO support.
  find_lto(CXX)

  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  enable_testing()

  if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_definitions(-DNDEBUG)
    add_definitions(-DPROJECT_DEBUG)
  endif()

  add_definitions(-DATM_COMPILATION)

  add_subdirectory(libutil)
  add_subdirectory(libcore)
  add_subdirectory(libnetworking)
  add_subdirectory(libdetector)
  add_subdirectory(libgui)
  add_subdirectory(libstorage)
  add_subdirectory(driver)
  add_subdirectory(app)
endif()

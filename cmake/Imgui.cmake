# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Glad.cmake)

# OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

CPMAddPackage(
  NAME imgui
  GITHUB_REPOSITORY ocornut/imgui
  GIT_TAG e8447dea453fe11c4f7da6512b86f4e039f03261
)

if(imgui_ADDED)

  set(imgui_sources
      "${imgui_SOURCE_DIR}/imgui.cpp"
      "${imgui_SOURCE_DIR}/imgui_draw.cpp"
      "${imgui_SOURCE_DIR}/imgui_demo.cpp"
      "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
      "${imgui_SOURCE_DIR}/examples/imgui_impl_glfw.cpp"
      "${imgui_SOURCE_DIR}/examples/imgui_impl_opengl3.cpp"
  )

  add_library(imgui STATIC ${imgui_sources})

  target_link_libraries(imgui PUBLIC glfw ${OPENGL_LIBRARIES} glad)

  target_include_directories(imgui PUBLIC "$<BUILD_INTERFACE:${imgui_SOURCE_DIR}>")
  target_compile_definitions(imgui PUBLIC GLFW_INCLUDE_NONE)
  target_compile_definitions(imgui PUBLIC OPENGL3_EXIST IMGUI_IMPL_OPENGL_LOADER_GLAD)
  set(IMGUI_FOUND ON)
endif()

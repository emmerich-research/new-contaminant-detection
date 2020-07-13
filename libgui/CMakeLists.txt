project(gui)

ucm_add_files(
  "${IMGUI_INCLUDE_DIR}/imgui.cpp"
  "${IMGUI_INCLUDE_DIR}/imgui_draw.cpp"
  "${IMGUI_INCLUDE_DIR}/imgui_demo.cpp"
  "${IMGUI_INCLUDE_DIR}/imgui_widgets.cpp"
  "${IMGUI_INCLUDE_DIR}/examples/imgui_impl_glfw.cpp"
  "${IMGUI_INCLUDE_DIR}/examples/imgui_impl_opengl3.cpp"

  "manager.cpp"
  "image_texture.cpp"

  "window.cpp"
  "image_window.cpp"
  # "init.cpp"
  # "util.cpp"
  # "widget.cpp"
  # "window.cpp"

  TO SOURCES)

ucm_add_target(
  NAME gui
  TYPE STATIC
  SOURCES ${SOURCES}
  UNITY CPP_PER_UNITY 20
  PCH_FILE "gui.hpp")

add_library("${PROJECT_NAMESPACE}::gui" ALIAS gui)

target_include_directories(gui PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
  "$<BUILD_INTERFACE:${OPENGL_INCLUDE_DIR}>"
  "$<BUILD_INTERFACE:${OPENGL_EGL_INCLUDE_DIRS}>"
  "$<BUILD_INTERFACE:${IMGUI_INCLUDE_DIR}>"
  "$<BUILD_INTERFACE:${GLAD_INCLUDE_DIRS}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_compile_definitions(gui PUBLIC
  GLFW_INCLUDE_NONE)

target_compile_definitions(gui PUBLIC
  OPENGL3_EXIST
  IMGUI_IMPL_OPENGL_LOADER_GLAD)

target_link_libraries(gui PUBLIC
  ${OpenCV_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_DL_LIBS}
  glfw
  ${OPENGL_LIBRARIES}
  ${GLAD_LIBRARIES}
  "${PROJECT_NAMESPACE}::util"
  "${PROJECT_NAMESPACE}::core")

target_set_warnings(gui
  ENABLE ALL
  # AS_ERROR ALL
  DISABLE Annoying)

set_target_properties(gui PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO)

target_compile_options(gui PRIVATE
  -Wno-extra-semi-stmt
  -Wno-alloca)

target_enable_lto(gui optimized)

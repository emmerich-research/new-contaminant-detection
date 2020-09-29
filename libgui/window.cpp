#include "gui.hpp"

#include "window.hpp"

NAMESPACE_BEGIN

namespace gui {
Window::Window(const char*             name,
               float                   width,
               float                   height,
               const ImGuiWindowFlags& flags)
    : name_{name}, width_{width}, height_{height}, flags_{flags} {
  ImGui::SetNextWindowSize(ImVec2{width, height});
}

Window::~Window() {}

void Window::render() {
  if (!ImGui::Begin(name(), nullptr, flags())) {
    ImGui::End();
    return;
  }

  show();

  ImGui::End();
}

void Window::after_render() {}
}  // namespace gui

NAMESPACE_END

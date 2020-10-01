#include "gui.hpp"

#include <algorithm>
#include <iterator>
#include <vector>

#include "image_window.hpp"

NAMESPACE_BEGIN

namespace gui {
ImageWindow::ImageWindow(const char*             name,
                         float                   height,
                         float                   width,
                         const ImGuiWindowFlags& flags)
    : Window{name, height, width, flags} {}

ImageWindow::~ImageWindow() {}

void ImageWindow::frame(cv::Mat* image) {
  texture().image(image);
}

void ImageWindow::show() {
  ImGui::Image(texture().texture(), texture().size());
}

void ImageWindow::after_render() {
  texture().destroy();
}
}  // namespace gui

NAMESPACE_END

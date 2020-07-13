#include "gui.hpp"

#include <algorithm>
#include <iterator>
#include <vector>

#include "image_texture.hpp"
#include "image_window.hpp"

NAMESPACE_BEGIN

namespace gui {
ImageWindow::ImageWindow(const char*             name,
                         float                   height,
                         float                   width,
                         const ImGuiWindowFlags& flags)
    : Window{name, height, width, flags} {}

ImageWindow::~ImageWindow() {}

void ImageWindow::frame(const cv::Mat& image) {
  frame_ = image;
}

void ImageWindow::show() {
  ImageTexture texture{frame()};

  ImGui::Image(texture.texture(), texture.size());
}
}  // namespace gui

NAMESPACE_END

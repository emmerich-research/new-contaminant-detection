#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <libcore/core.hpp>
#include <libdetector/detector.hpp>
#include <libgui/gui.hpp>
#include <libnetworking/networking.hpp>
#include <libutil/util.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  USE_NAMESPACE

  if (initialize_core()) {
    return ATM_ERR;
  }

  gui::Manager ui_manager;

  ui_manager.init("Emmerich Vision", 400, 400);

  if (!ui_manager.active()) {
    return ATM_ERR;
  }

  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    std::cout << "camera cannot be opened" << std::endl;
    return ATM_ERR;
  }

  ui_manager.key_callback([](gui::Manager::MainWindow* current_window, int key,
                             [[maybe_unused]] int scancode, int action,
                             int mods) {
    if (mods == GLFW_MOD_ALT && key == GLFW_KEY_F4 && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(current_window, GL_TRUE);
    } else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(current_window, GL_TRUE);
    }
  });

  ImGuiWindowFlags window_flags = 0;
  // window_flags |= ImGuiWindowFlags_NoTitleBar;
  // window_flags |= ImGuiWindowFlags_NoScrollbar;
  // window_flags |= ImGuiWindowFlags_NoMove;
  // window_flags |= ImGuiWindowFlags_NoResize;
  window_flags |= ImGuiWindowFlags_NoCollapse;
  // window_flags |= ImGuiWindowFlags_NoNav;
  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
  window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

  detector::BlobDetector blob_detector;

  // create windows
  gui::ImageWindow* image_window = new gui::ImageWindow("image", 300, 300);
  gui::ImageWindow* blob_window =
      new gui::ImageWindow("blob-detection", 300, 300);

  // add window
  ui_manager.add_window(image_window);
  ui_manager.add_window(blob_window);

  while (ui_manager.handle_events()) {
    cv::Mat frame, blob;
    if (cap.read(frame)) {
      // make halfsize image
      // cv::resize(frame, frame, cv::Size{0, 0}, 0.5, 0.5, cv::INTER_LINEAR);

      // gain
      frame.convertTo(frame, CV_8U, 1.0, 0);

      // show halfsize image
      image_window->frame(frame);

      blob_detector.detect(std::move(frame), std::move(blob));
      blob_window->frame(blob);

      ui_manager.render();
    }
  }

  cap.release();
  ui_manager.exit();

  return 0;
}

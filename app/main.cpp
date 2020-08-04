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
#include <libstorage/storage.hpp>
#include <libutil/util.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  USE_NAMESPACE

  if (initialize_core()) {
    std::cerr << "cannot initialize config, state, and logger!" << std::endl;
    return ATM_ERR;
  }

  cv::VideoCapture cap{Config::get()->camera_idx(), cv::CAP_V4L2};
  if (!cap.isOpened()) {
    LOG_ERROR("Camera cannot be opened!");
    return ATM_ERR;
  }

  networking::ModbusConfig config{Config::get()};
  const auto&              timeout = config.timeout();

  networking::Modbus::ErrorCode ec;
  networking::modbus::TCP       modbus{config.host().c_str(), config.port(),
                                 timeout.connect, timeout.request,
                                 timeout.response};

  ec = modbus.connect();

  if (ec) {
    LOG_ERROR("Connection error, message: {}", ec.message());
    return ATM_ERR;
  }

  cv::Mat                  frame;
  cv::Mat                  blob;
  storage::StorageListener storage_listener{
      reinterpret_cast<const networking::ModbusConfig*>(&config), &modbus,
      reinterpret_cast<const cv::Mat*>(&frame),
      /** autorun */ true};
  networking::ModbusListener modbus_listener{
      reinterpret_cast<const networking::ModbusConfig*>(&config), &modbus,
      /** autorun */ true};

  // detector::BlobDetector blob_detector;

  gui::Manager ui_manager;

  ui_manager.init("Emmerich Vision", 400, 400);

  if (!ui_manager.active()) {
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

  ui_manager.error_callback([](int error, const char* description) {
    LOG_ERROR("Glfw Error {}: {}", error, description);
  });

  // create windows
  gui::ImageWindow* image_window = new gui::ImageWindow("image", 300, 300);
  // gui::ImageWindow* blob_window =
  //     new gui::ImageWindow("blob-detection", 300, 300);

  // add windows
  ui_manager.add_window(image_window);
  // ui_manager.add_window(blob_window);
  ui_manager.add_window<networking::modbus::ModbusWindow>(config);

  while (ui_manager.handle_events()) {
    if (cap.read(frame)) {
      frame.convertTo(frame, CV_8U, 1.0, 0);

      // show
      image_window->frame(&frame);

      // blob_detector.detect(std::move(frame), std::move(blob));
      // blob_window->frame(&blob);

      ui_manager.render();
    }
  }

  storage_listener.stop();
  modbus_listener.stop();

  ec = modbus.close();

  if (ec) {
    LOG_ERROR("Cannot close connection to Modbus!");
  }

  cap.release();
  ui_manager.exit();

  return 0;
}

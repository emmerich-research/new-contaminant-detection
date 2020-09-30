#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <modbuscpp/modbus.hpp>

#include <libcore/core.hpp>
#include <libdetector/detector.hpp>
#include <libgui/gui.hpp>
#include <libserver/server.hpp>
#include <libstorage/storage.hpp>
#include <libutil/util.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  USE_NAMESPACE

  if (initialize_core()) {
    std::cerr << "cannot initialize config, state, and logger!" << std::endl;
    return ATM_ERR;
  }

  auto*          config = Config::get();
  server::Config server_config{config};

  cv::VideoCapture cap{config->camera_idx(), cv::CAP_V4L2};
  if (!cap.isOpened()) {
    LOG_ERROR("Camera cannot be opened!");
    return ATM_ERR;
  }

  server::Slave          slave(&server_config);
  server::DataMapper     data_mapper{&server_config, &slave};
  cv::Mat                frame, blob;
  detector::BlobDetector blob_detector;
  gui::Manager           ui_manager;

  // listeners
  storage::StorageListener storage_listener{&server_config, &data_mapper,
                                            &frame};

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
  ui_manager.add<gui::ImageWindow>("image", "image", 300, 300);
  ui_manager.add<gui::ImageWindow>("blob", "blob-detection", 300, 300);
  ui_manager.add<server::DataWindow>("data", &server_config, &data_mapper);

  auto* image_window = dynamic_cast<gui::ImageWindow*>(ui_manager.get("image"));
  auto* blob_window = dynamic_cast<gui::ImageWindow*>(ui_manager.get("blob"));

  LOG_INFO("Running server...");
  slave.run();

  LOG_INFO("Running listeners...");
  storage_listener.start();

  while (ui_manager.handle_events()) {
    if (cap.read(frame)) {
      frame.convertTo(frame, CV_8U, 1.0, 0);

      // show
      image_window->frame(&frame);

      blob_detector.detect(std::move(frame), std::move(blob));
      blob_window->frame(&blob);

      ui_manager.render();
    }
  }

  storage_listener.stop();
  slave.stop();
  cap.release();
  ui_manager.exit();

  return 0;
}

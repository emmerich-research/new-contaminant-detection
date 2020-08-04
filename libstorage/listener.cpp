#include "storage.hpp"

#include "listener.hpp"

#include <opencv4/opencv2/opencv.hpp>

#include <boost/uuid/uuid_io.hpp>

#include <libnetworking/networking.hpp>
#include <libutil/util.hpp>

NAMESPACE_BEGIN

namespace storage {
StorageListener::StorageListener(const networking::ModbusConfig* config,
                                 networking::Modbus*             modbus,
                                 const cv::Mat*                  image,
                                 bool                            autorun)
    : config_{config},
      modbus_{modbus},
      database_{config->base_config()->images_db()},
      image_{image} {
  fs::create_directory(config->base_config()->images_dir());
  if (autorun) {
    start();
  }
}

StorageListener::~StorageListener() {
  running_ = false;
  if (thread().joinable()) {
    thread().join();
  }
}

void StorageListener::start() {
  Listener::LockGuard lock(mutex());
  if (!running()) {
    LOG_INFO("Starting storage listener");
    running_ = true;
    thread_ = std::thread(&StorageListener::execute, this);
  }
}

void StorageListener::stop() {
  Listener::LockGuard lock(mutex());
  if (running()) {
    LOG_INFO("Stopping storage listener");
    running_ = false;
    if (thread().joinable()) {
      thread().join();
    }
    LOG_INFO("Stopping storage listener complete");
  }
}

void StorageListener::write_status(const std::string& key, bool value) {
  massert(State::get() != nullptr, "sanity");

  auto* state = State::get();

  if (key.compare(IMAGING_READY_KEY) == 0) {
    const auto& imaging_ready = config()->jetson_plc_comm(IMAGING_READY_KEY);
    modbus()->write_bit(imaging_ready.address, value);
  } else if (key.compare(IMAGING_DONE_KEY) == 0) {
    const auto& imaging_done = config()->jetson_plc_comm(IMAGING_DONE_KEY);
    modbus()->write_bit(imaging_done.address, value);
  }

  state->status_table(key, value);
}

void StorageListener::execute() {
  massert(State::get() != nullptr, "sanity");

  auto* state = State::get();

  write_status(IMAGING_READY_KEY, true);

  while (running()) {
    while (running() && !state->status_table("imaging-request")) {
      // waiting
    }

    if (!running()) {
      break;
    }

    LOG_DEBUG("Saving images...");

    std::string   hash = boost::uuids::to_string(generator());
    schema::Image image_entry{
        database().last_id() + 1,          hash,
        state->data_table("year"),         state->data_table("month"),
        state->data_table("day"),          state->data_table("hour"),
        state->data_table("minute"),       state->data_table("second"),
        state->data_table("sku_card"),     state->data_table("sku_number"),
        state->data_table("sku_prod_day"), state->data_table("tray_barcode"),
        state->data_table("lid_barcode"),  state->data_table("batch_id"),
        state->data_table("infection_id")};

    write_status(IMAGING_READY_KEY, false);
    write_status(IMAGING_DONE_KEY, false);

    // save data
    database().insert(image_entry);
    imwrite(
        fmt::format("{}/{}.jpg", config()->base_config()->images_dir(), hash),
        *(image()));

    write_status(IMAGING_DONE_KEY, true);
    // wait for 3s before listening again
    sleep_for<time_units::seconds>(3);
    write_status(IMAGING_READY_KEY, true);
    write_status(IMAGING_DONE_KEY, false);
    // wait for 5s before listening again
    sleep_for<time_units::seconds>(5);
  }
}
}  // namespace storage

NAMESPACE_END

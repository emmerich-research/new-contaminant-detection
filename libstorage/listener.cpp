#include "storage.hpp"

#include "listener.hpp"

#include <opencv4/opencv2/opencv.hpp>

#include <boost/uuid/uuid_io.hpp>

#include <libserver/server.hpp>
#include <libutil/util.hpp>

NAMESPACE_BEGIN

namespace storage {
StorageListener::StorageListener(const server::Config* config,
                                 server::DataMapper*   data_mapper,
                                 const cv::Mat*        image,
                                 bool                  autorun)
    : config_{config},
      data_mapper_{data_mapper},
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
  data_mapper_->status(server::mapping::alt_type_t::jetson_status, key, value);
}

long long StorageListener::read_data(const std::string& key) const {
  return data_mapper_->data(server::mapping::alt_type_t::plc_data, key);
}

void StorageListener::execute() {
  massert(State::get() != nullptr, "sanity");

  write_status(IMAGING_READY_KEY, true);

  while (running()) {
    while (running() &&
           !data_mapper_->status(server::mapping::alt_type_t::plc_status,
                                 "imaging-request")) {
      sleep_for<time_units::millis>(100);  // waiting
    }

    if (!running()) {
      break;
    }

    std::string hash = boost::uuids::to_string(generator());

    LOG_INFO("Saving image with hash={}...", hash);

    schema::Image image_entry{database().last_id() + 1,
                              hash,
                              read_data("year"),
                              read_data("month"),
                              read_data("day"),
                              read_data("hour"),
                              read_data("minute"),
                              read_data("second"),
                              read_data("sku-card-instruction"),
                              read_data("sku-number"),
                              read_data("sku-production-day"),
                              read_data("tray-barcode"),
                              read_data("lid-barcode"),
                              read_data("batch-id"),
                              read_data("infection-id")};

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

    LOG_INFO("Image has been saved");
  }
}
}  // namespace storage

NAMESPACE_END

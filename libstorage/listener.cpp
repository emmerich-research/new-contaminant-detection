#include "storage.hpp"

#include "listener.hpp"

#include <fmt/format.h>

#include <opencv4/opencv2/opencv.hpp>

#include <libserver/server.hpp>
#include <libutil/util.hpp>

#include "database.hpp"

NAMESPACE_BEGIN

namespace storage {
static std::string sku_card_instruction(long long sku_card_instr) {
  if (sku_card_instr == 0) {
    return "Colonize";
  } else if (sku_card_instr == 1) {
    return "TEND";
  } else if (sku_card_instr == 2) {
    return "ExposeFlip";
  } else if (sku_card_instr == 3) {
    return "FlipNylonFlip";
  } else if (sku_card_instr == 4) {
    return "TendFlipFlipCompFlip";
  } else if (sku_card_instr == 5) {
    return "FlipTend";
  } else if (sku_card_instr == 8) {
    return "HarvestBed";
  } else {
    return "NOT_USED";
  }
}

static std::string sku_number(long long sku_num) {
  if (sku_num == 0) {
    return "REGEN";
  } else if (sku_num == 1) {
    return "ESKIMO";
  } else if (sku_num == 2) {
    return "TIGER";
  } else if (sku_num == 3) {
    return "SAPIEN";
  } else {
    return "NOT_USED";
  }
}

static std::string lid_barcode(long long lid_barcode_id) {
  auto lid_barcode_str = std::to_string(lid_barcode_id);

  if (lid_barcode_str.length() != 7) {
    return lid_barcode_str;
  }

  auto lid_type_num = lid_barcode_str.substr(0, 3);
  auto lid_id = lid_barcode_str.substr(3);

  std::string lid_type = "UNK";

  if (lid_type_num.compare("000") == 0) {
    lid_type = "NO_LID";
  } else if (lid_type_num.compare("101") == 0) {
    lid_type = "CL:HP1";
  } else if (lid_type_num.compare("102") == 0) {
    lid_type = "CL:HP2";
  } else if (lid_type_num.compare("103") == 0) {
    lid_type = "CL:HP3";
  } else if (lid_type_num.compare("201") == 0) {
    lid_type = "GL:HP1";
  } else if (lid_type_num.compare("202") == 0) {
    lid_type = "GL:HP2";
  } else if (lid_type_num.compare("203") == 0) {
    lid_type = "GL:HP3";
  } else if (lid_type_num.compare("999") == 0) {
    lid_type = "NO_HOLES";
  }

  return fmt::format("{}-{}", lid_type, lid_id);
}

static std::string infection(long long infection_num) {
  if (infection_num == 0) {
    return "NONE";
  } else if (infection_num == 1) {
    return "BLUE/GREEN";
  } else if (infection_num == 2) {
    return "BLACK/BROWN";
  } else if (infection_num == 3) {
    return "METABOLITE";
  } else {
    return "NOT_USED";
  }
}
StorageListener::StorageListener(const server::Config* config,
                                 server::DataMapper*   data_mapper,
                                 Database*             database,
                                 const cv::Mat*        image,
                                 bool                  autorun)
    : config_{config},
      data_mapper_{data_mapper},
      database_{database},
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

    auto year = read_data("year");
    auto month = read_data("month");
    auto day = read_data("day");
    auto hour = read_data("hour");
    auto minute = read_data("minute");
    auto second = read_data("second");
    auto raw_sku_card_instr = read_data("sku-card-instruction");
    auto raw_sku_num = read_data("sku-number");
    auto sku_prod_day = read_data("sku-production-day");
    auto tray_barcode = read_data("tray-barcode");
    auto raw_lid_barcode = read_data("lid-barcode");
    auto batch_id = read_data("batch-id");
    auto raw_infection_id = read_data("infection-id");

    auto hash = fmt::format("TRAY_{}_{}-{}-{}_{}-{}-{}", tray_barcode, month,
                            day, year, hour, minute, second);

    schema::Image image_entry{-1,
                              hash,
                              year,
                              month,
                              day,
                              hour,
                              minute,
                              second,
                              sku_card_instruction(raw_sku_card_instr),
                              sku_number(raw_sku_num),
                              sku_prod_day,
                              tray_barcode,
                              lid_barcode(raw_lid_barcode),
                              batch_id,
                              infection(raw_infection_id)};

    LOG_INFO("Saving {}", image_entry);

    write_status(IMAGING_READY_KEY, false);
    write_status(IMAGING_DONE_KEY, false);

    // save data
    database_->insert(image_entry);
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

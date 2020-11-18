#include "cloud.hpp"

#include "listener.hpp"

#include <libserver/server.hpp>
#include <libstorage/storage.hpp>
#include <libutil/util.hpp>

#include "config.hpp"
#include "database.hpp"
#include "storage.hpp"

NAMESPACE_BEGIN

namespace cloud {
CloudListener::CloudListener(const Config*      config,
                             storage::Database* internal_db,
                             Database*          cloud_db,
                             Storage*           storage,
                             bool               autorun)
    : config_{config},
      internal_db_{internal_db},
      cloud_db_{cloud_db},
      storage_{storage} {
  if (autorun) {
    start();
  }
}

CloudListener::~CloudListener() {
  running_ = false;
  if (thread().joinable()) {
    thread().join();
  }
}

void CloudListener::start() {
  Listener::LockGuard lock(mutex());
  if (!running()) {
    LOG_INFO("Starting cloud listener");
    running_ = true;
    thread_ = std::thread(&CloudListener::execute, this);
  }
}

void CloudListener::stop() {
  Listener::LockGuard lock(mutex());
  if (running()) {
    LOG_INFO("Stopping cloud listener");
    running_ = false;
    if (thread().joinable()) {
      thread().join();
    }
    LOG_INFO("Stopping cloud listener complete");
  }
}

void CloudListener::execute() {
  massert(State::get() != nullptr, "sanity");

  while (running()) {
    while (running() && internal_db_->empty()) {
      // check every 1s
      sleep_for<time_units::seconds>(1);
    }

    if (!running()) {
      break;
    }

    if (!internal_db_->empty()) {
      // upload to database frequently
      auto img = internal_db_->first();

      LOG_DEBUG("Getting {}", img);

      // always delete if image exists in storage
      storage_->remove(img.hash);

      if (storage_->insert(img.hash)) {
        try {
          cloud_db_->insert(img);
          internal_db_->remove(img.hash);
          storage_->update_metadata(img.hash);
          fs::remove(fmt::format(
              "{}/{}.jpg", config_->base_config()->images_dir(), img.hash));
        } catch (...) {
          storage_->remove(img.hash);
        }
      }

      // int  retry = 0;
      // while (!storage_->insert(img.hash) && retry < 10) {
      //   retry++;
      //   sleep_for<time_units::millis>(250);
      // }

      // retry = 0;

      // try {
      //   cloud_db_->insert(img);
      //   internal_db_->remove(img.hash);
      // } catch (...) {
      //   while (!storage_->remove(img.hash) && retry < 10) {
      //     retry++;
      //     sleep_for<time_units::millis>(250);
      //   }
      // }
    }
  }
}
}  // namespace cloud

NAMESPACE_END

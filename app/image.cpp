#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <libcloud/cloud.hpp>
#include <libcore/core.hpp>
#include <libstorage/storage.hpp>
#include <libutil/util.hpp>

int main([[maybe_unused]] int argc, char* argv[]) {
  USE_NAMESPACE

  if (argc != 3) {
    std::cerr << "should pass 2 parameter either remove or upload and filename"
              << std::endl;
    return ATM_ERR;
  }

  if (initialize_core()) {
    std::cerr << "cannot initialize config, state, and logger!" << std::endl;
    return ATM_ERR;
  }

  // config
  auto*          config = Config::get();
  cloud::Config  cloud_config{config};
  cloud::Storage cloud_storage(&cloud_config);

  if (!cloud_storage.active()) {
    LOG_ERROR("Cannot initialize connection to cloud storage");
    return ATM_ERR;
  }

  if (strcmp(argv[1], "remove") == 0) {
    cloud_storage.insert(argv[2]);
  } else if (strcmp(argv[1], "upload") == 0) {
    cloud_storage.insert(argv[2]);
  } else if (strcmp(argv[1], "metadata") == 0) {
    cloud_storage.update_metadata(argv[2]);
  } else {
    LOG_INFO("Parameter should be only remove or upload");
  }
}

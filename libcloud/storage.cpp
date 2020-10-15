#include "cloud.hpp"

#include "storage.hpp"

#include <fmt/format.h>

#include <fstream>

#include "config.hpp"

NAMESPACE_BEGIN

namespace cloud {
Storage::Storage(const Config* config)
    : config_{config},
      active_{true},
      client_{gcs::Client::CreateDefaultClient()} {
  if (!client_) {
    active_ = false;
    LOG_INFO("Cannot initialize google cloud config, message={}",
             client_.status());
  }
}

Storage::~Storage() {}

bool Storage::insert(const storage::schema::Hash& hash) {
  auto filename =
      fmt::format("{}/{}.jpg", config_->base_config()->images_dir(), hash);
  auto obj_name = fmt::format("{}.jpg", hash);

  LOG_DEBUG("Uploading {} to {} in bucket {}", filename, obj_name,
            config_->storage_bucket());

  std::ifstream img(filename, std::ios::binary);

  auto stream = client_->WriteObject(config_->storage_bucket(), obj_name);
  stream << img.rdbuf();
  stream.Close();

  auto metadata = std::move(stream).metadata();

  if (!metadata) {
    LOG_ERROR("UploadFile Error: {}", metadata.status().message());
    return false;
  }

  LOG_DEBUG("Uploaded {} to object {} in bucket {}", filename, metadata->name(),
            metadata->bucket());
  return true;
}

bool Storage::remove(const storage::schema::Hash& hash) {
  auto obj_name = fmt::format("{}", hash);
  auto status = client_->DeleteObject(config_->storage_bucket(), obj_name);

  if (status.ok()) {
    LOG_DEBUG("Removed object {} in bucket {}", obj_name,
              config_->storage_bucket());
    return true;
  } else {
    LOG_ERROR("DeleteFile Error: {}", status.message());
    return false;
  }
}
}  // namespace cloud

NAMESPACE_END

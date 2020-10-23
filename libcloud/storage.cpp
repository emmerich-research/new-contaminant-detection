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
      client_{gcs::Client(*gcs::ClientOptions::CreateDefaultClientOptions(),
                          gcs::LimitedErrorCountRetryPolicy(3))} {
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

  auto stream = client_->WriteObject(
      config_->storage_bucket(), obj_name, gcs::IfGenerationMatch(0),
      gcs::IfMetagenerationMatch(),
      gcs::WithObjectMetadata(
          gcs::ObjectMetadata().set_content_type("image/jpeg")));
  stream << img.rdbuf();
  stream.Close();

  auto metadata = stream.metadata();

  if (!metadata) {
    LOG_ERROR("UploadFile Error: {}", metadata.status().message());
    return false;
  }

  LOG_DEBUG("Uploaded {} to object {} in bucket {}", filename, metadata->name(),
            metadata->bucket());

  return true;
}

bool Storage::remove(const storage::schema::Hash& hash) {
  auto obj_name = fmt::format("{}.jpg", hash);
  auto status = client_->DeleteObject(config_->storage_bucket(), obj_name);

  if (status.ok()) {
    LOG_DEBUG("Removed object {} in bucket {}", obj_name,
              config_->storage_bucket());
    return true;
  } else {
    LOG_DEBUG("DeleteFile Error: {}", status.message());
    return false;
  }
}

void Storage::update_metadata(const storage::schema::Hash& hash) {
  auto obj_name = fmt::format("{}.jpg", hash);
  auto obj_metadata =
      client_->GetObjectMetadata(config_->storage_bucket(), obj_name);

  if (!obj_metadata) {
    LOG_DEBUG("GetObjectMetadata Error: name={}, obj_name={}", obj_name,
              obj_metadata.status().message());
  }

  auto desired = *obj_metadata;
  desired.set_content_type("image/jpeg");

  auto updated =
      client_->UpdateObject(config_->storage_bucket(), obj_name, desired);

  if (!updated) {
    LOG_DEBUG("UpdateMetadata Error: name={}, message={}", obj_name,
              updated.status().message());
  }
}
}  // namespace cloud

NAMESPACE_END

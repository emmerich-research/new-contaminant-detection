#ifndef LIB_CLOUD_STORAGE_HPP_
#define LIB_CLOUD_STORAGE_HPP_

#include <memory>
#include <string>

#include <libcore/core.hpp>
#include <libstorage/storage.hpp>

#include <google/cloud/storage/client.h>

NAMESPACE_BEGIN

namespace gcs = google::cloud::storage;

namespace cloud {
class Config;

class Storage {
 public:
  /**
   * Storage constructor
   *
   * @param config cloud config
   */
  Storage(const Config* config);

  /**
   * Storage destructor
   */
  ~Storage();

  /**
   * Upload image to storage
   *
   * @param hash image hash
   */
  bool insert(const storage::schema::Hash& hash);

  /**
   * Remove image from storage
   *
   * @param hash image hash
   */
  bool remove(const storage::schema::Hash& hash);

  /**
   * Update metadata
   *
   * @param hash image hash
   */
  void update_metadata(const storage::schema::Hash& hash);

  /**
   * Storage active status
   *
   * @return active status
   */
  bool active() const { return active_; }

 private:
  /**
   * Config
   */
  const Config* config_;
  /**
   * Active
   */
  bool active_;
  /**
   * Google cloud storage client
   */
  google::cloud::StatusOr<gcs::Client> client_;
};
}  // namespace cloud

NAMESPACE_END

#endif  // LIB_CLOUD_STORAGE_HPP_

#ifndef LIB_STORAGE_LISTENER_HPP_
#define LIB_STORAGE_LISTENER_HPP_

#include <boost/uuid/random_generator.hpp>

#include <libcore/core.hpp>

#include "internal-database.hpp"

// forward declarations
namespace cv {
class Mat;
}

NAMESPACE_BEGIN

namespace server {
class Config;
}  // namespace server

namespace storage {
class StorageListener : public Listener {
 public:
  StorageListener(const server::Config* config,
                  const cv::Mat*        image,
                  bool                  autorun = false);
  virtual ~StorageListener() override;

  virtual void start() override;
  virtual void stop() override;

 private:
  void execute();

  inline const Database& database() const { return database_; }
  inline Database&       database() { return database_; }

  inline const cv::Mat* image() const { return image_; }

  inline const server::Config* config() const { return config_; }

  void write_status(const std::string& key, bool value);

 private:
  const server::Config*           config_;
  Database                        database_;
  const cv::Mat*                  image_;
  boost::uuids::random_generator  generator;
};
}  // namespace storage

NAMESPACE_END

#endif  // LIB_STORAGE_LISTENER_HPP_

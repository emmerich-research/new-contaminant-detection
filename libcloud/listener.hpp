#ifndef LIB_CLOUD_LISTENER_HPP_
#define LIB_CLOUD_LISTENER_HPP_

#include <thread>

#include <libcore/core.hpp>

NAMESPACE_BEGIN

namespace storage {
class Database;
}

namespace cloud {
class Config;
class Database;
class Storage;

class CloudListener : public Listener {
 public:
  /**
   * CloudListener constructor
   *
   * @param config      cloud configuration
   * @param internal_db internal database
   * @param cloud_db    cloud database
   * @param autorun     autorun listener
   */
  CloudListener(const Config*      config,
                storage::Database* internal_db,
                Database*          cloud_db,
                Storage*           storage,
                bool               autorun = false);

  /**
   * CloudListener destructor
   */
  virtual ~CloudListener() override;

  /**
   * Start cloud listener
   */
  virtual void start() override;

  /**
   * Stop cloud listener
   */
  virtual void stop() override;

 private:
  /**
   * Execute task
   */
  void execute();

 private:
  /**
   * Configuration
   */
  const Config* config_;
  /**
   * Internal database
   */
  storage::Database* internal_db_;
  /**
   * Cloud database
   */
  Database* cloud_db_;
  /**
   * Cloud storage
   */
  Storage* storage_;
};
}  // namespace cloud

NAMESPACE_END

#endif  // LIB_CLOUD_LISTENER_HPP_

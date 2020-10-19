#ifndef LIB_CLOUD_DATABASE_HPP_
#define LIB_CLOUD_DATABASE_HPP_

#include <memory>
#include <string>

#include <tao/pq.hpp>

#include <libcore/core.hpp>
#include <libstorage/storage.hpp>

NAMESPACE_BEGIN

namespace cloud {
class Config;

class Database : public storage::Database {
 public:
  /**
   * Database constructor
   *
   * @param config storage config
   */
  Database(const Config* config);

  /**
   * Database destructor
   */
  virtual ~Database() override;

  /**
   * Get number of entries in the database
   *
   * @return number of entries
   */
  virtual int num_entries() override;

  /**
   * Get first image
   *
   * @return image object
   */
  virtual storage::schema::Image first() override;

  /**
   * Get image by hash
   *
   * @param hash imamge hash
   *
   * @return image object
   */
  virtual storage::schema::Image get(
      const storage::schema::Hash& hash) override;

  /**
   * Insert image schema to database
   *
   * @param image image metadata
   */
  virtual void insert(const storage::schema::Image& image) override;

  /**
   * Remove image from database with specified hash
   *
   * @param hash image hash
   */
  virtual void remove(const storage::schema::Hash& hash) override;

  /**
   * Check whether database empty
   *
   * @return database empty status
   */
  virtual bool empty() override;

  /**
   * Check if image exist
   *
   * @param hash hash to check
   */
  virtual bool check(const storage::schema::Hash& hash) override;

  /**
   * Health check
   *
   * @return health check
   */
  inline virtual bool active() override;

  /**
   * Prepare statements
   */
  void prepare_statements();

  /**
   * Get connection pointer
   *
   * @return connection pointer
   */
  inline tao::pq::connection* connection() { return connection_.get(); }

 private:
  /**
   * Connect to database
   */
  void connect();

 private:
  /**
   * Connection active
   */
  bool active_;
  /**
   * Cloud config
   */
  const Config* config_;
  /**
   * Connection
   */
  std::shared_ptr<tao::pq::connection> connection_;
};
}  // namespace cloud

NAMESPACE_END

#endif  // LIB_CLOUD_DATABASE_HPP_

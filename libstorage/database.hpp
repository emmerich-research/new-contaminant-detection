#ifndef LIB_STORAGE_DATABASE_HPP_
#define LIB_STORAGE_DATABASE_HPP_

#include <condition_variable>
#include <cstdint>
#include <string>

#include <sqlite3.h>
#include <sqlite_orm/sqlite_orm.h>

#include <libcore/core.hpp>

#include "schema.hpp"

NAMESPACE_BEGIN

namespace storage {
class Database {
 public:
  /**
   * Database constructor
   */
  Database();

  /**
   * Database destructor
   */
  virtual ~Database();

  /**
   * Insert image schema to database
   *
   * @param image image metadata
   */
  virtual void insert(const schema::Image& image) = 0;

  /**
   * Remove image from database with specified hash
   *
   * @param hash image hash
   */
  virtual void remove(const schema::Hash& hash) = 0;

  /**
   * Get image by hash
   *
   * @param hash imamge hash
   *
   * @return image object
   */
  inline schema::Image operator[](const schema::Hash& hash) {
    return get(hash);
  }

  /**
   * Get image by hash
   *
   * @param hash image hash
   *
   * @return image object
   */
  virtual schema::Image get(const schema::Hash& hash) = 0;

  /**
   * Get first image
   *
   * @return image object
   */
  virtual schema::Image first() = 0;

  /**
   * Check if image exist by hash
   *
   * @param hash hash to check
   *
   * @return true if image exists
   */
  virtual bool check(const schema::Hash& hash) = 0;

  /**
   * Check whether database empty
   *
   * @return database empty status
   */
  virtual bool empty() = 0;

  /**
   * Get number of entries in the database
   *
   * @return number of entries
   */
  virtual int num_entries() = 0;

  /**
   * Check whether database is active or not
   *
   * @return active
   */
  inline virtual bool active() { return true; }
};

class InternalDatabase : public Database {
 public:
  /**
   * Database constructor
   *
   * @param path database path
   */
  InternalDatabase(const std::string& path);

  /**
   * Database destructor
   */
  virtual ~InternalDatabase() override;

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
  virtual schema::Image first() override;

  /**
   * Get image by hash
   *
   * @param hash imamge hash
   *
   * @return image object
   */
  virtual schema::Image get(const schema::Hash& hash) override;

  /**
   * Insert image schema to database
   *
   * @param image image metadata
   */
  virtual void insert(const schema::Image& image) override;

  /**
   * Remove image from database with specified hash
   *
   * @param hash image hash
   */
  virtual void remove(const schema::Hash& hash) override;

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

 protected:
  /**
   * Get database path
   *
   * @return database path
   */
  inline const std::string& path() const { return path_; }

  /**
   * Get database storage
   *
   * @return database storage
   */
  inline Storage& storage() { return storage_; }

  /**
   * Get database storage (const)
   *
   * @return database storage (const)
   */
  inline const Storage& storage() const { return storage_; }

 private:
  /**
   * Internal database path
   */
  const std::string path_;
  /**
   * Storage
   */
  Storage storage_;
};
}  // namespace storage

NAMESPACE_END

#endif  // LIB_STORAGE_DATABASE_HPP_

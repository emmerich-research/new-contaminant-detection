#ifndef LIB_STORAGE_INTERNAL_DATABASE_HPP_
#define LIB_STORAGE_INTERNAL_DATABASE_HPP_

#include <cstdint>
#include <string>

#include <sqlite3.h>
#include <sqlite_orm/sqlite_orm.h>

#include <libcore/core.hpp>

#include "schema.hpp"

NAMESPACE_BEGIN

namespace storage {
class Database : public StackObj {
 public:
  Database(const std::string& path);
  ~Database();

  inline const std::string& path() const { return path_; }

  inline Storage&       storage() { return storage_; }
  inline const Storage& storage() const { return storage_; }

  int  num_entries();
  int  last_id();
  void insert(const schema::Image& image);
  void remove(const schema::PrimaryKey& key);

 private:
  const std::string path_;
  Storage           storage_;
};
}  // namespace storage

NAMESPACE_END

#endif  // LIB_STORAGE_INTERNAL_DATABASE_HPP_

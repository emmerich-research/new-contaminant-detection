#include "storage.hpp"

#include "database.hpp"

#include <libutil/util.hpp>

NAMESPACE_BEGIN

namespace storage {
static Storage init_storage(const std::string& path) {
  using namespace sqlite_orm;
  fs::path p = path;
  auto     parent_path = p.parent_path();
  if (!parent_path.empty())
    fs::create_directory(parent_path);
  return sqlite_orm::make_storage(
      path,
      sqlite_orm::make_table(
          "IMAGES",
          sqlite_orm::make_column("ID", &schema::Image::id, autoincrement(),
                                  primary_key()),
          sqlite_orm::make_column("HASH", &schema::Image::hash, unique()),
          sqlite_orm::make_column("YEAR", &schema::Image::year),
          sqlite_orm::make_column("MONTH", &schema::Image::month),
          sqlite_orm::make_column("DAY", &schema::Image::day),
          sqlite_orm::make_column("HOUR", &schema::Image::hour),
          sqlite_orm::make_column("MINUTE", &schema::Image::minute),
          sqlite_orm::make_column("SECOND", &schema::Image::second),
          sqlite_orm::make_column("SKU_CARD", &schema::Image::sku_card),
          sqlite_orm::make_column("SKU_NUMBER", &schema::Image::sku_number),
          sqlite_orm::make_column("SKU_PROD_DAY", &schema::Image::sku_prod_day),
          sqlite_orm::make_column("TRAY_BARCODE", &schema::Image::tray_barcode),
          sqlite_orm::make_column("LID_BARCODE", &schema::Image::lid_barcode),
          sqlite_orm::make_column("BATCH_ID", &schema::Image::batch_id),
          sqlite_orm::make_column("INFECTION_ID",
                                  &schema::Image::infection_id)));
}

Database::Database() {}

Database::~Database() {}

InternalDatabase::InternalDatabase(const std::string& path)
    : path_{std::move(path)}, storage_{init_storage(path)} {
  storage().sync_schema();
}

InternalDatabase::~InternalDatabase() {}

schema::Image InternalDatabase::get(const schema::Hash& hash) {
  using namespace sqlite_orm;
  auto images = storage().select(&schema::Image::id,
                                 where(is_equal(&schema::Image::hash, hash)));
  return storage().get<schema::Image>(images[0]);
}

void InternalDatabase::insert(const schema::Image& image) {
  storage().insert(image);
}

schema::Image InternalDatabase::first() {
  using namespace sqlite_orm;
  auto users = storage().get_all<schema::Image>(limit(1));
  return users.front();
}

void InternalDatabase::remove(const schema::Hash& hash) {
  using namespace sqlite_orm;
  storage().remove_all<schema::Image>(
      where(is_equal(&schema::Image::hash, hash)));
}

int InternalDatabase::num_entries() {
  return storage().count<schema::Image>();
}

bool InternalDatabase::empty() {
  return num_entries() == 0;
}

bool InternalDatabase::check(const storage::schema::Hash& hash) {
  using namespace sqlite_orm;
  auto images = storage().select(&schema::Image::hash,
                                 where(is_equal(&schema::Image::hash, hash)));

  return images.size() == 1;
}
}  // namespace storage

NAMESPACE_END

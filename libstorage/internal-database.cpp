#include "storage.hpp"

#include "internal-database.hpp"

#include <libutil/util.hpp>

NAMESPACE_BEGIN

namespace storage {
static Storage init_storage(const std::string& path) {
  fs::path p = path;
  auto     parent_path = p.parent_path();
  if (!parent_path.empty())
    fs::create_directory(parent_path);
  return sqlite_orm::make_storage(
      path,
      sqlite_orm::make_table(
          "IMAGES",
          sqlite_orm::make_column("ID", &schema::Image::id,
                                  sqlite_orm::primary_key()),
          sqlite_orm::make_column("HASH", &schema::Image::hash),
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

Database::Database(const std::string& path)
    : path_{std::move(path)}, storage_{init_storage(path)} {
  storage().sync_schema();
}

Database::~Database() {}

void Database::insert(const schema::Image& image) {
  storage().insert(image);
}

void Database::remove(const schema::PrimaryKey& key) {
  storage().remove<schema::Image>(key);
}

int Database::num_entries() {
  return storage().count<schema::Image>();
}

int Database::last_id() {
  if (auto id = storage().max(&schema::Image::id)) {
    return *id;
  }

  return 0;
}
}  // namespace storage

NAMESPACE_END

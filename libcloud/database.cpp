#include "cloud.hpp"

#include "database.hpp"

#include <cassert>
#include <stdexcept>
#include <utility>

#include <fmt/format.h>

#include <tao/pq/table_writer.hpp>

#include "config.hpp"

NAMESPACE_BEGIN

namespace cloud {
/**
 psql "sslmode=verify-ca sslrootcert=server-ca.pem \
      sslcert=client-cert.pem sslkey=client-key.pem \
      hostaddr=<host> \
      port=5432 \
      user=<user> dbname=postgres"
*/

Database::Database(const Config* config)
    : config_{config},
      connection_{tao::pq::connection::create(
          fmt::format("sslmode=verify-ca sslrootcert={} sslcert={} sslkey={} "
                      "hostaddr={} port={} user={} dbname={}",
                      config->database_ssl().root_cert,
                      config->database_ssl().client_cert,
                      config->database_ssl().client_key,
                      config->database_host(),
                      config->database_port(),
                      config->database_user(),
                      config->database_name()))} {
  prepare_statements();
}

Database::~Database() {}

void Database::prepare_statements() {
  if (active()) {
    connection_->prepare(
        "insert",
        "INSERT INTO images(hash, year, month, day, hour, minute, second, "
        "sku_card, sku_number, sku_prod_day, tray_barcode, lid_barcode, "
        "batch_id, infection) VALUES ( $1, $2, $3, $4, $5, $6, $7, $8, "
        "$9, $10, $11, $12, $13, $14 )");
    connection_->prepare("remove", "DELETE FROM images WHERE HASH=$1");
    connection_->prepare("exist",
                         "SELECT EXISTS( SELECT 1 FROM IMAGES WHERE HASH=$1) ");
    connection_->prepare("get", "SELECT * FROM IMAGES WHERE HASH=$1");
    connection_->prepare("count",
                         "SELECT 100 * count(*) AS estimate FROM images "
                         "TABLESAMPLE SYSTEM (1);");
    connection_->prepare("first", "SELECT * FROM images LIMIT 1;");
  }
}

bool Database::active() const {
  auto res = connection_->execute(
      "SELECT EXISTS ("
      "SELECT FROM pg_catalog.pg_class c "
      "JOIN   pg_catalog.pg_namespace n ON n.oid = c.relnamespace "
      "WHERE  n.nspname = 'public' "
      "AND    c.relname = 'images' "
      "AND    c.relkind = 'r');");
  return res.as<bool>();
}

storage::schema::Image Database::first() {
  auto res = connection_->execute("first");

  if (res.empty()) {
    throw std::runtime_error("Cannot get first image");
  }

  auto value = res[0];

  return {value["id"].as<int>(),
          value["hash"].as<storage::schema::Hash>(),
          value["year"].as<long long>(),
          value["month"].as<long long>(),
          value["day"].as<long long>(),
          value["hour"].as<long long>(),
          value["minute"].as<long long>(),
          value["second"].as<long long>(),
          value["sku_card"].as<std::string>(),
          value["sku_number"].as<std::string>(),
          value["sku_prod_day"].as<long long>(),
          value["tray_barcode"].as<long long>(),
          value["lid_barcode"].as<std::string>(),
          value["batch_id"].as<long long>(),
          value["batch_id"].as<std::string>()};
}

storage::schema::Image Database::get(const storage::schema::Hash& hash) {
  auto res = connection_->execute("get", hash);

  if (res.empty()) {
    throw std::runtime_error(fmt::format("Cannot get image by hash={}", hash));
  }

  auto value = res[0];

  return {value["id"].as<int>(),
          value["hash"].as<storage::schema::Hash>(),
          value["year"].as<long long>(),
          value["month"].as<long long>(),
          value["day"].as<long long>(),
          value["hour"].as<long long>(),
          value["minute"].as<long long>(),
          value["second"].as<long long>(),
          value["sku_card"].as<std::string>(),
          value["sku_number"].as<std::string>(),
          value["sku_prod_day"].as<long long>(),
          value["tray_barcode"].as<long long>(),
          value["lid_barcode"].as<std::string>(),
          value["batch_id"].as<long long>(),
          value["batch_id"].as<std::string>()};
}

void Database::insert(const storage::schema::Image& image) {
  const auto tr = connection_->transaction();
  tr->execute("insert", image.hash, image.year, image.month, image.day,
              image.hour, image.minute, image.second, image.sku_card,
              image.sku_number, image.sku_prod_day, image.tray_barcode,
              image.lid_barcode, image.batch_id, image.infection_id);
  tr->commit();
}

void Database::remove(const storage::schema::Hash& hash) {
  const auto tr = connection_->transaction();
  tr->execute("remove", hash);
  tr->commit();
}

int Database::num_entries() {
  auto res = connection_->execute("count");
  return res.as<int>();
}

bool Database::empty() {
  return num_entries() == 0;
}

bool Database::check(const storage::schema::Hash& hash) {
  auto res = connection_->execute("exist", hash);
  return res.as<bool>();
}
}  // namespace cloud

NAMESPACE_END

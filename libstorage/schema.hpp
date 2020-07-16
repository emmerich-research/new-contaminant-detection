#ifndef LIB_STORAGE_SCHEMA_HPP_
#define LIB_STORAGE_SCHEMA_HPP_

#include <cstdint>
#include <string>

#include <sqlite_orm/sqlite_orm.h>

#include <libcore/core.hpp>

NAMESPACE_BEGIN

namespace storage {
namespace schema {
using PrimaryKey = int;
using Hash = std::string;

struct Image {
  PrimaryKey   id;
  Hash         hash;
  unsigned int year;
  unsigned int month;
  unsigned int day;
  unsigned int hour;
  unsigned int minute;
  unsigned int second;
  unsigned int sku_card;
  unsigned int sku_number;
  unsigned int sku_prod_day;
  unsigned int heartbit;
  unsigned int tray_barcode;
  unsigned int lid_barcode;
  unsigned int batch_id;
  unsigned int infection_id;
};
}  // namespace schema

template <class O, class T, class... Op>
using Column = sqlite_orm::internal::
    column_t<O, T, const T& (O::*)() const, void (O::*)(T), Op...>;

using Storage = sqlite_orm::internal::storage_t<sqlite_orm::internal::table_t<
    schema::Image,
    Column<schema::Image,
           decltype(schema::Image::id),
           sqlite_orm::constraints::primary_key_t<>>,
    Column<schema::Image, decltype(schema::Image::hash)>,
    Column<schema::Image, decltype(schema::Image::year)>,
    Column<schema::Image, decltype(schema::Image::month)>,
    Column<schema::Image, decltype(schema::Image::day)>,
    Column<schema::Image, decltype(schema::Image::hour)>,
    Column<schema::Image, decltype(schema::Image::minute)>,
    Column<schema::Image, decltype(schema::Image::second)>,
    Column<schema::Image, decltype(schema::Image::sku_card)>,
    Column<schema::Image, decltype(schema::Image::sku_number)>,
    Column<schema::Image, decltype(schema::Image::sku_prod_day)>,
    Column<schema::Image, decltype(schema::Image::heartbit)>,
    Column<schema::Image, decltype(schema::Image::tray_barcode)>,
    Column<schema::Image, decltype(schema::Image::lid_barcode)>,
    Column<schema::Image, decltype(schema::Image::batch_id)>,
    Column<schema::Image, decltype(schema::Image::infection_id)>>>;
}  // namespace storage

NAMESPACE_END

#endif // LIB_STORAGE_SCHEMA_HPP_

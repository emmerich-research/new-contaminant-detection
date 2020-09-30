#include "server.hpp"

#include "data-mapper.hpp"

#include <iterator>

#include <modbuscpp/modbus.hpp>

#include "slave.hpp"

NAMESPACE_BEGIN

namespace server {
static long long convert_uint16_to_ll(
    data::type_t                                                    type,
    const modbus::block::registers::container_type::const_iterator& begin,
    const modbus::block::registers::container_type::const_iterator& end) {
  auto diff = std::distance(begin, end);

  if (diff == 1 &&
      ((type == data::type_t::byte) || (type == data::type_t::word))) {
    return static_cast<long long>(*(begin));
  } else if (diff == 2 && (type == data::type_t::dword)) {
    return static_cast<long long>(
        (static_cast<std::int32_t>(*(begin + 1)) << 16) |
        static_cast<std::int32_t>(*(begin)));
  } else if (diff == 4 && (type == data::type_t::lword)) {
    return static_cast<long long>(
        (static_cast<std::int64_t>(*(begin + 3)) << 48) |
        (static_cast<std::int64_t>(*(begin + 2)) << 32) |
        (static_cast<std::int64_t>(*(begin + 1)) << 16) |
        static_cast<std::int64_t>(*(begin)));
  }

  return -1;
}

DataMapper::DataMapper(const Config* config, Slave* slave)
    : config_{config}, slave_{slave} {}

DataMapper::~DataMapper() {}

long long DataMapper::data(mapping::alt_type_t type, std::string id) const {
  switch (type) {
    case mapping::alt_type_t::plc_data: {
      auto& meta = config_->plc_data(id);
      auto [start, end] = slave_->data_table().holding_registers().get(
          modbus::address_t{meta.address},
          modbus::read_num_regs_t{meta.length});
      return convert_uint16_to_ll(meta.type, start, end);
    }
    case mapping::alt_type_t::jetson_data: {
      auto& meta = config_->jetson_data(id);
      auto [start, end] = slave_->data_table().input_registers().get(
          modbus::address_t{meta.address},
          modbus::read_num_regs_t{meta.length});
      return convert_uint16_to_ll(meta.type, start, end);
    }
    default:
      return 0;
  }
}

bool DataMapper::status(mapping::alt_type_t type, std::string id) const {
  switch (type) {
    case mapping::alt_type_t::plc_status: {
      auto& meta = config_->plc_status(id);
      return slave_->data_table().coils().get(modbus::address_t{meta.address});
    }
    case mapping::alt_type_t::jetson_status: {
      auto& meta = config_->jetson_status(id);
      return slave_->data_table().discrete_inputs().get(
          modbus::address_t{meta.address});
    }
    default:
      return false;
  }
}

void DataMapper::status(mapping::alt_type_t type, std::string id, bool value) {
  switch (type) {
    case mapping::alt_type_t::plc_status: {
      auto& meta = config_->plc_status(id);
      slave_->data_table().coils().set(modbus::address_t{meta.address}, value);
      break;
    }
    case mapping::alt_type_t::jetson_status: {
      auto& meta = config_->jetson_status(id);
      slave_->data_table().discrete_inputs().set(
          modbus::address_t{meta.address}, value);
      break;
    }
    default:
      break;
  }
}
}  // namespace server

NAMESPACE_END

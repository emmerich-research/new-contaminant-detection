#include "networking.hpp"

#include "modbus-config.hpp"

#include <algorithm>
#include <limits>

namespace toml {
template <>
struct from<ns(networking::modbus::Metadata)> {
  template <typename C,
            template <typename...>
            class M,
            template <typename...>
            class A>
  static ns(networking::modbus::Metadata)
      from_toml(const basic_value<C, M, A>& v) {
    ns(networking::modbus::Metadata) metadata;
    metadata.address = 0;
    if (v.contains("address")) {
      metadata.address = find<std::uint16_t>(v, "address") - 1;
    }
    metadata.length = 1;
    if (v.contains("length")) {
      metadata.length = find<std::uint16_t>(v, "length");
    }
    return metadata;
  }
};

template <>
struct from<ns(networking::modbus::Timeout)> {
  template <typename C,
            template <typename...>
            class M,
            template <typename...>
            class A>
  static ns(networking::modbus::Timeout)
      from_toml(const basic_value<C, M, A>& v) {
    ns(networking::modbus::Timeout) timeout;
    timeout.connect = ns(networking::Modbus::MaxTimeout);
    if (v.contains("connect")) {
      timeout.connect = ns(
          networking::modbus::time::seconds(find<std::uint16_t>(v, "connect")));
    }

    timeout.request = ns(networking::Modbus::MaxTimeout);
    if (v.contains("request")) {
      timeout.request = ns(
          networking::modbus::time::seconds(find<std::uint16_t>(v, "request")));
    }

    timeout.response = ns(networking::Modbus::MaxTimeout);
    if (v.contains("response")) {
      timeout.response = ns(networking::modbus::time::seconds(
          find<std::uint16_t>(v, "response")));
    }
    return timeout;
  }
};
}  // namespace toml

NAMESPACE_BEGIN

namespace networking {

ModbusConfig::ModbusConfig(const impl::ConfigImpl* config)
    : base_config_{config} {
  massert(config != nullptr, "sanity");
  load();
}

void ModbusConfig::load() {
  load_master();
  timeout_ = base_config()->find<modbus::Timeout>("modbus", "timeout");
  load_data();
  load_plc_jetson_comm();
  load_jetson_plc_comm();
}

void ModbusConfig::load_master() {
  host_ = base_config()->find<std::string>("modbus", "ip-address");
  port_ =
      static_cast<std::uint16_t>(base_config()->find<int>("modbus", "port"));
  LOG_DEBUG("Port {}", port());
}

void ModbusConfig::load_data() {
  massert(State::get() != nullptr, "sanity");
  auto* state = State::get();

  min_data_address_ = std::numeric_limits<std::uint16_t>::max();
  max_data_address_ = std::numeric_limits<std::uint16_t>::min();

  for (const auto& [k, v] :
       base_config()->find<toml::table>("modbus", "data")) {
    if (k != "type") {
      state->data_table(k, -1);
      auto metadata =
          base_config()->find<modbus::Metadata>("modbus", "data", k);
      // data_[k] = std::move(metadata);
      data_.push_back({k, metadata});

      if (min_data_address_ > metadata.address) {
        min_data_address_ = metadata.address;
      }

      if (max_data_address_ < metadata.address + metadata.length) {
        max_data_address_ = metadata.address + metadata.length;
      }
    }
  }

  data_length_ = max_data_address() - min_data_address() + 1;

  std::sort(data().begin(), data().end(),
            [=](const ModbusConfig::TableEntry& a,
                const ModbusConfig::TableEntry& b) {
              return a.second.address < b.second.address;
            });
}

void ModbusConfig::load_plc_jetson_comm() {
  massert(State::get() != nullptr, "sanity");
  auto* state = State::get();

  min_plc_jetson_comm_address_ = std::numeric_limits<std::uint16_t>::max();
  max_plc_jetson_comm_address_ = std::numeric_limits<std::uint16_t>::min();

  for (const auto& [k, v] : base_config()->find<toml::table>(
           "modbus", "communication", "plc-jetson")) {
    if (k != "type") {
      state->status_table(k, false);
      auto metadata = base_config()->find<modbus::Metadata>(
          "modbus", "communication", "plc-jetson", k);
      plc_jetson_comm_.push_back({k, metadata});

      if (min_plc_jetson_comm_address_ > metadata.address) {
        min_plc_jetson_comm_address_ = metadata.address;
      }

      if (max_plc_jetson_comm_address_ < metadata.address + metadata.length) {
        max_plc_jetson_comm_address_ = metadata.address + metadata.length;
      }
    }
  }

  plc_jetson_comm_length_ =
      max_plc_jetson_comm_address() - min_plc_jetson_comm_address() + 1;

  std::sort(plc_jetson_comm().begin(), plc_jetson_comm().end(),
            [=](const ModbusConfig::TableEntry& a,
                const ModbusConfig::TableEntry& b) {
              return a.second.address < b.second.address;
            });
}

void ModbusConfig::load_jetson_plc_comm() {
  massert(State::get() != nullptr, "sanity");
  auto* state = State::get();

  min_jetson_plc_comm_address_ = std::numeric_limits<std::uint16_t>::max();
  max_jetson_plc_comm_address_ = std::numeric_limits<std::uint16_t>::min();

  for (const auto& [k, v] : base_config()->find<toml::table>(
           "modbus", "communication", "jetson-plc")) {
    if (k != "type") {
      state->status_table(k, false);
      auto metadata = base_config()->find<modbus::Metadata>(
          "modbus", "communication", "jetson-plc", k);

      // jetson_plc_comm_[k] = std::move(metadata);
      jetson_plc_comm_.push_back({k, metadata});

      if (min_jetson_plc_comm_address_ > metadata.address) {
        min_jetson_plc_comm_address_ = metadata.address;
      }

      if (max_jetson_plc_comm_address_ < metadata.address + metadata.length) {
        max_jetson_plc_comm_address_ = metadata.address + metadata.length;
      }
    }
  }

  jetson_plc_comm_length_ =
      max_jetson_plc_comm_address() - min_jetson_plc_comm_address() + 1;

  std::sort(jetson_plc_comm().begin(), jetson_plc_comm().end(),
            [=](const ModbusConfig::TableEntry& a,
                const ModbusConfig::TableEntry& b) {
              return a.second.address < b.second.address;
            });
}

const modbus::Metadata& ModbusConfig::data(const std::string& id) const {
  auto it = std::find_if(data().begin(), data().end(),
                         [&id](const ModbusConfig::TableEntry& entry) {
                           return entry.first == id;
                         });
  return (*it).second;
  // return data_.at(id);
}

const modbus::Metadata& ModbusConfig::plc_jetson_comm(
    const std::string& id) const {
  auto it = std::find_if(plc_jetson_comm().begin(), plc_jetson_comm().end(),
                         [&id](const ModbusConfig::TableEntry& entry) {
                           return entry.first == id;
                         });
  return (*it).second;
  // return plc_jetson_comm_.at(id);
}

const modbus::Metadata& ModbusConfig::jetson_plc_comm(
    const std::string& id) const {
  auto it = std::find_if(
      jetson_plc_comm().begin(), jetson_plc_comm().end(),
      [=](const ModbusConfig::TableEntry& entry) { return entry.first == id; });
  return (*it).second;
  // return jetson_plc_comm_.at(id);
}
}  // namespace networking

NAMESPACE_END

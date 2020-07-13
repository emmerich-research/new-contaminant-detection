#include "networking.hpp"

#include "modbus-config.hpp"

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
}  // namespace toml

NAMESPACE_BEGIN

namespace networking {

ModbusConfig::ModbusConfig(const impl::ConfigImpl* config)
    : base_config_{config} {
  massert(config != nullptr, "sanity");
  load();
}

void ModbusConfig::load() {
  load_data();
  load_plc_jetson_comm();
  load_jetson_plc_comm();
}

void ModbusConfig::load_data() {
  massert(State::get() != nullptr, "sanity");
  auto* state = State::get();

  for (const auto& [k, v] : base_config()->find<toml::table>("data")) {
    if (k != "type") {
      state->data_table(k, -1);
      data_keys_.push_back(k);
      data_[k] = base_config()->find<modbus::Metadata>("data", k);
    }
  }
}

void ModbusConfig::load_plc_jetson_comm() {
  massert(State::get() != nullptr, "sanity");
  auto* state = State::get();

  for (const auto& [k, v] :
       base_config()->find<toml::table>("communication", "plc-jetson")) {
    if (k != "type") {
      state->status_table(k, false);
      plc_jetson_comm_keys_.push_back(k);
      plc_jetson_comm_[k] = base_config()->find<modbus::Metadata>(
          "communication", "plc-jetson", k);
    }
  }
}

void ModbusConfig::load_jetson_plc_comm() {
  massert(State::get() != nullptr, "sanity");
  auto* state = State::get();

  for (const auto& [k, v] :
       base_config()->find<toml::table>("communication", "jetson-plc")) {
    if (k != "type") {
      state->status_table(k, false);
      jetson_plc_comm_keys_.push_back(k);
      jetson_plc_comm_[k] = base_config()->find<modbus::Metadata>(
          "communication", "jetson-plc", k);
    }
  }
}

const modbus::Metadata& ModbusConfig::data_table(const std::string& id) const {
  return data_.at(id);
}

const modbus::Metadata& ModbusConfig::plc_jetson_comm_table(
    const std::string& id) const {
  return plc_jetson_comm_.at(id);
}

const modbus::Metadata& ModbusConfig::jetson_plc_comm_table(
    const std::string& id) const {
  return jetson_plc_comm_.at(id);
}
}  // namespace networking

NAMESPACE_END

#include "server.hpp"

#include "config.hpp"

#include <algorithm>
#include <limits>

namespace toml {
template <>
struct from<ns(server::data::meta_t)> {
  template <typename C,
            template <typename...>
            class M,
            template <typename...>
            class A>
  static ns(server::data::meta_t) from_toml(const basic_value<C, M, A>& v) {
    ns(server::data::meta_t) metadata;
    metadata.address = 0;
    if (v.contains("address")) {
      metadata.address = find<std::uint16_t>(v, "address");
    }
    metadata.length = 1;
    if (v.contains("length")) {
      metadata.length = find<std::uint16_t>(v, "length");
    }
    metadata.type = ns(server::data::type_t::byte);
    if (v.contains("type")) {
      auto type = find<std::string>(v, "type");
      if ((type.compare("WORD") == 0) || (type.compare("word") == 0)) {
        metadata.type = ns(server::data::type_t::word);
      } else if ((type.compare("DWORD") == 0) || (type.compare("dword") == 0)) {
        metadata.type = ns(server::data::type_t::dword);
      } else if ((type.compare("LWORD") == 0) || (type.compare("lword") == 0)) {
        metadata.type = ns(server::data::type_t::lword);
      }
    }
    return metadata;
  }
};

template <>
struct from<ns(server::mapping::data_t)> {
  template <typename C,
            template <typename...>
            class M,
            template <typename...>
            class A>
  static ns(server::mapping::data_t) from_toml(const basic_value<C, M, A>& v) {
    ns(server::mapping::data_t) data_mapping;
    std::uint16_t min_address = std::numeric_limits<std::uint16_t>::max();
    std::uint16_t max_address = std::numeric_limits<std::uint16_t>::min();

    // metadata
    if (v.contains("starting-address")) {
      data_mapping.meta.starting_address =
          find<std::uint16_t>(v, "starting-address");
    }

    if (v.contains("data")) {
      for (const auto& [k, _] : find<toml::table>(v, "data")) {
        auto metadata = find<ns(server::data::meta_t)>(v, "data", k);
        data_mapping.info.push_back({k, metadata});

        if (min_address > metadata.address) {
          min_address = metadata.address;
        }

        if (max_address < metadata.address + metadata.length) {
          max_address = metadata.address + metadata.length;
        }
      }
    }

    data_mapping.meta.capacity = max_address - min_address + 1;

    // capacity
    if (v.contains("capacity")) {
      data_mapping.meta.capacity = find<std::uint16_t>(v, "capacity");
    }

    return data_mapping;
  }
};
}  // namespace toml

NAMESPACE_BEGIN

namespace server {
Config::Config(const impl::ConfigImpl* config) : base_config_{config} {
  massert(config != nullptr, "sanity");
  load();
}

void Config::sort_mapping_info(mapping::info_t& info) {
  std::sort(info.begin(), info.end(),
            [=](const mapping::entry_t& a, const mapping ::entry_t& b) {
              return a.second.address < b.second.address;
            });
}

void Config::load() {
  load_server_info();
  load_data();
}

void Config::load_server_info() {
  port_ =
      static_cast<std::uint16_t>(base_config()->find<int>("modbus", "port"));
}

void Config::load_data_helper(const char*            key,
                              const mapping::type_t& type,
                              mapping::data_t&       mapping_data) {
  mapping_data =
      base_config()->find<mapping::data_t>("modbus", "data-mapping", key);
  mapping_data.type = type;
  sort_mapping_info(mapping_data.info);
}

void Config::load_data() {
  load_data_helper("input-registers", mapping::type_t::input_regs,
                   jetson_data_);
  load_data_helper("discrete-inputs", mapping::type_t::discrete_inputs,
                   jetson_status_);

  load_data_helper("holding-registers", mapping::type_t::holding_regs,
                   plc_data_);
  load_data_helper("coils", mapping::type_t::coils, plc_status_);
}

const data::meta_t& Config::jetson_data(const std::string& id) const {
  auto it = std::find_if(
      jetson_data().info.begin(), jetson_data().info.end(),
      [&id](const mapping::entry_t& entry) { return entry.first == id; });
  return (*it).second;
}

const data::meta_t& Config::jetson_status(const std::string& id) const {
  auto it = std::find_if(
      jetson_status().info.begin(), jetson_status().info.end(),
      [&id](const mapping::entry_t& entry) { return entry.first == id; });

  return (*it).second;
}

const data::meta_t& Config::plc_data(const std::string& id) const {
  auto it = std::find_if(
      plc_data().info.begin(), plc_data().info.end(),
      [&id](const mapping::entry_t& entry) { return entry.first == id; });
  return (*it).second;
}

const data::meta_t& Config::plc_status(const std::string& id) const {
  auto it = std::find_if(
      plc_status().info.begin(), plc_status().info.end(),
      [&id](const mapping::entry_t& entry) { return entry.first == id; });
  return (*it).second;
}
}  // namespace server

NAMESPACE_END

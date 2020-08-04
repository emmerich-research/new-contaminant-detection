#ifndef LIB_NETWORKING_MODBUS_CONFIG_HPP_
#define LIB_NETWORKING_MODBUS_CONFIG_HPP_

/** @file modbus-config.hpp
 *  @brief Modbus config implementation
 *
 * Modbus config Implementation
 */

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <libcore/core.hpp>

#include "modbus.hpp"

NAMESPACE_BEGIN

namespace networking {
namespace modbus {
enum class DataType : unsigned int { BYTE = 0, WORD, DWORD, LWORD };

struct Metadata {
  DataType      type;
  std::uint16_t address;
  std::uint16_t length;
};

struct Timeout {
  Modbus::Timeout connect;
  Modbus::Timeout request;
  Modbus::Timeout response;
};
}  // namespace modbus

class ModbusConfig {
 public:
  typedef std::pair<std::string, modbus::Metadata> TableEntry;
  typedef std::vector<TableEntry>                  Table;

  ModbusConfig(const impl::ConfigImpl* config);
  ModbusConfig(const ModbusConfig& modbus_config) = default;

  inline const impl::ConfigImpl* base_config() const { return base_config_; }

  inline Table& data() { return data_; };
  inline Table& plc_jetson_comm() { return plc_jetson_comm_; };
  inline Table& jetson_plc_comm() { return jetson_plc_comm_; };

  inline const Table& data() const { return data_; };
  inline const Table& plc_jetson_comm() const { return plc_jetson_comm_; };
  inline const Table& jetson_plc_comm() const { return jetson_plc_comm_; };

  const modbus::Metadata& data(const std::string& id) const;
  const modbus::Metadata& plc_jetson_comm(const std::string& id) const;
  const modbus::Metadata& jetson_plc_comm(const std::string& id) const;

  inline const std::string&   host() const { return host_; }
  inline const std::uint16_t& port() const { return port_; }

  inline const modbus::Timeout& timeout() const { return timeout_; }

  inline const std::uint16_t& min_data_address() const {
    return min_data_address_;
  }

  inline const std::uint16_t& max_data_address() const {
    return max_data_address_;
  }

  inline const std::uint16_t& data_length() const { return data_length_; }

  inline const std::uint16_t& min_plc_jetson_comm_address() const {
    return min_plc_jetson_comm_address_;
  }

  inline const std::uint16_t& max_plc_jetson_comm_address() const {
    return max_plc_jetson_comm_address_;
  }

  inline const std::uint16_t& plc_jetson_comm_length() const {
    return plc_jetson_comm_length_;
  }

  inline const std::uint16_t& min_jetson_plc_comm_address() const {
    return min_jetson_plc_comm_address_;
  }

  inline const std::uint16_t& max_jetson_plc_comm_address() const {
    return max_jetson_plc_comm_address_;
  }

  inline const std::uint16_t& jetson_plc_comm_length() const {
    return jetson_plc_comm_length_;
  }

 private:
  void load();
  void load_master();
  void load_data();
  void load_plc_jetson_comm();
  void load_jetson_plc_comm();

  static void sort_table(Table& table);

 private:
  const impl::ConfigImpl* base_config_;

  Table data_;
  Table plc_jetson_comm_;
  Table jetson_plc_comm_;

  std::string   host_;
  std::uint16_t port_;

  modbus::Timeout timeout_;

  std::uint16_t min_data_address_;
  std::uint16_t max_data_address_;
  std::uint16_t data_length_;

  std::uint16_t min_plc_jetson_comm_address_;
  std::uint16_t max_plc_jetson_comm_address_;
  std::uint16_t plc_jetson_comm_length_;

  std::uint16_t min_jetson_plc_comm_address_;
  std::uint16_t max_jetson_plc_comm_address_;
  std::uint16_t jetson_plc_comm_length_;
};
}  // namespace networking

NAMESPACE_END

#endif  // LIB_NETWORKING_MODBUS_CONFIG_HPP_

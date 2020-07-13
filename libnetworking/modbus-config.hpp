#ifndef LIB_NETWORKING_MODBUS_CONFIG_HPP_
#define LIB_NETWORKING_MODBUS_CONFIG_HPP_

/** @file modbus-config.hpp
 *  @brief Modbus implementation
 *
 * Modbus Implementation
 */

#include <cstdint>
#include <map>
#include <string>

#include <libcore/core.hpp>

NAMESPACE_BEGIN

namespace networking {
namespace modbus {
enum class type { coil, input_coil, holding_register, input_register };

struct Metadata {
  // std::string   type;
  std::uint16_t address;
  std::uint16_t length;
};
}  // namespace modbus

class ModbusConfig {
 public:
  typedef std::map<std::string, modbus::Metadata> Table;

  ModbusConfig(const impl::ConfigImpl* config);
  ~ModbusConfig();

  inline const impl::ConfigImpl* base_config() const { return base_config_; }

  const modbus::Metadata& data_table(const std::string& id) const;
  const modbus::Metadata& plc_jetson_comm_table(const std::string& id) const;
  const modbus::Metadata& jetson_plc_comm_table(const std::string& id) const;

 private:
  void load();
  void load_data();
  void load_plc_jetson_comm();
  void load_jetson_plc_comm();

 private:
  const impl::ConfigImpl* base_config_;
  Table                   data_;
  Table                   plc_jetson_comm_;
  Table                   jetson_plc_comm_;
};
}  // namespace networking

NAMESPACE_END

#endif  // LIB_NETWORKING_MODBUS_CONFIG_HPP_

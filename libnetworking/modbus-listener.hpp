#ifndef LIB_NETWORKING_MODBUS_LISTENER_HPP_
#define LIB_NETWORKING_MODBUS_LISTENER_HPP_

/** @file modbus-listener.hpp
 *  @brief Modbus listener implementation
 *
 * Modbus listener Implementation
 */

#include <cstdint>

#include <libcore/core.hpp>

#include "modbus.hpp"

#include "modbus-config.hpp"

NAMESPACE_BEGIN

namespace networking {
class ModbusListener : public Listener {
 public:
  ModbusListener(const ModbusConfig* config, bool autorun = false);
  ModbusListener(const ModbusConfig* config,
                 Modbus*             modbus,
                 bool                autorun = false);
  virtual ~ModbusListener() override;

  virtual void start() override;
  virtual void stop() override;

  inline const Modbus* modbus() const { return modbus_; }
  inline Modbus*       modbus() { return modbus_; }

 private:
  inline const ModbusConfig* config() const { return config_; }

  inline bool modbus_allocation() const { return modbus_allocation_; }

  void error_callback(const ModbusError& error);
  void response_callback(const ModbusResponse& response);

  void execute();

 private:
  const ModbusConfig* config_;
  Modbus*            modbus_;
  const bool         modbus_allocation_;
};
}  // namespace networking

NAMESPACE_END

#endif  // LIB_NETWORKING_MODBUS_LISTENER_HPP_

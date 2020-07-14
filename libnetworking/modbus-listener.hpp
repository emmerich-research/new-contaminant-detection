#ifndef LIB_NETWORKING_MODBUS_LISTENER_HPP_
#define LIB_NETWORKING_MODBUS_LISTENER_HPP_

/** @file modbus-listener.hpp
 *  @brief Modbus listener implementation
 *
 * Modbus listener Implementation
 */

#include <condition_variable>
#include <cstdint>
#include <thread>

#include <libcore/core.hpp>

#include "modbus.hpp"

#include "modbus-config.hpp"

NAMESPACE_BEGIN

namespace networking {
namespace modbus {
class Listener : public StackObj {
 public:
  Listener(const ModbusConfig& config, bool autorun = false);
  virtual ~Listener();

  void start();
  void stop();

  inline const Modbus* modbus() const { return modbus_; }
  inline Modbus*       modbus() { return modbus_; }

 private:
  inline const ModbusConfig& config() const { return config_; }

  // inline const std::condition_variable& cv() const { return cv_; }
  // inline std::condition_variable&       cv() { return cv_; }

  inline const std::thread& thread() const { return thread_; }
  inline std::thread&       thread() { return thread_; }

  inline const bool& running() const { return running_; }

  inline const std::mutex& mutex() const { return mutex_; }
  inline std::mutex&       mutex() { return mutex_; }

  void error_callback(const ModbusError& error);
  void response_callback(const ModbusResponse& response);

  void execute();

 private:
  const ModbusConfig config_;
  bool               running_;
  Modbus*            modbus_;

  std::mutex  mutex_;
  std::thread thread_;
  // std::condition_variable cv_;
};
}  // namespace modbus
}  // namespace networking

NAMESPACE_END

#endif  // LIB_NETWORKING_MODBUS_LISTENER_HPP_

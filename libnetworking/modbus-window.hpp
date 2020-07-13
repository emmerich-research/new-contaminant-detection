#ifndef LIB_NETWORKING_MODBUS_WINDOW_HPP_
#define LIB_NETWORKING_MODBUS_WINDOW_HPP_

#include <libcore/core.hpp>
#include <libgui/gui.hpp>

#include "modbus-config.hpp"

NAMESPACE_BEGIN

namespace networking {
namespace modbus {
class ModbusWindow : public gui::Window {
 public:
  /**
   * Modbus Window constructor
   *
   * @param config modbus configuration
   * @param width  window width
   * @param height window height
   * @param flags  window flags
   */
  ModbusWindow(const ModbusConfig&     config,
               float                   height = 300.0f,
               float                   width = 300.0f,
               const ImGuiWindowFlags& flags = 0);
  /**
   * Modbus Window destructor
   */
  virtual ~ModbusWindow() override;
  /**
   *  Show contents
   */
  virtual void show() override;

 private:
  inline const ModbusConfig& config() const { return config_; }

 private:
  static const ImVec4 label_color;
  static const ImVec4 data_color;
  static const ImVec4 inactive_color;
  static const ImVec4 active_color;
  const ModbusConfig config_;
};
}  // namespace modbus
}  // namespace networking

NAMESPACE_END

#endif  // LIB_NETWORKING_MODBUS_WINDOW_HPP_

#ifndef LIB_SERVER_LOGGER_HPP_
#define LIB_SERVER_LOGGER_HPP_

/** @file logger.hpp
 *  @brief Modbus server logger
 */

#include <modbuscpp/modbus.hpp>

#include <libcore/core.hpp>

NAMESPACE_BEGIN

namespace server {
namespace internal {
class Logger : public modbus::logger {
 public:
  /**
   * Logger constructor
   *
   * @param debug debug status
   */
  explicit Logger(bool debug = false);

  /**
   * Logger destructor
   *
   * @param debug debug status
   */
  virtual ~Logger() override;

 protected:
  /**
   * Log level error
   *
   * @param message message to log
   */
  virtual void error_impl(const std::string& message) const noexcept override;

  /**
   * Log level debug
   *
   * @param message message to log
   */
  virtual void debug_impl(const std::string& message) const noexcept override;

  /**
   * Log level info
   *
   * @param message message to log
   */
  virtual void info_impl(const std::string& message) const noexcept override;
};
}  // namespace internal
}  // namespace server

NAMESPACE_END

#endif  // LIB_SERVER_LOGGER_HPP_

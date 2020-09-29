#ifndef LIB_SERVER_SLAVE_HPP_
#define LIB_SERVER_SLAVE_HPP_

/** @file slave.hpp
 *  @brief Modbus slave
 *
 * Modbus slave
 */

#include <boost/core/noncopyable.hpp>

#include <modbuscpp/modbus.hpp>

#include <libcore/core.hpp>

#include "config.hpp"

NAMESPACE_BEGIN

namespace server {
class Slave : private boost::noncopyable {
 public:
  /**
   * Modbus slave wrapper
   *
   * @param config    server config
   */
  Slave(const Config* config);

  /**
   * Run server slave
   */
  void run();

 private:
  /**
   * Init Modbus slave
   */
  void init();

 private:
  /**
   * Config pointer
   */
  const Config* config_;
  /**
   * Modbus server
   */
  modbus::server::pointer server_;
};
}  // namespace server

NAMESPACE_END

#endif  // LIB_SERVER_SLAVE_HPP_

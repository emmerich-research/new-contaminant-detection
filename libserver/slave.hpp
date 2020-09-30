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
   * Connection callback type
   */
  using conn_cb_t = modbus::server::conn_cb_t;

  /**
   * Modbus slave wrapper constructor
   *
   * @param config    server config
   */
  Slave(const Config* config);

  /**
   * Modbus slave destructor
   */
  ~Slave();

  /**
   * Run Modbus slave server
   */
  void run();

  /**
   * Stop Modbus slave server
   */
  void stop();

  /**
   * Get data table
   *
   * @return data table
   */
  inline modbus::table& data_table() { return server_->data_table(); }

  /**
   * Get data table (const)
   *
   * @return data table (const)
   */
  inline const modbus::table& data_table() const {
    return server_->data_table();
  }

 private:
  /**
   * Init Modbus slave
   */
  void init();

  /**
   * Connect callback
   *
   * @param session client session
   * @param table   data table
   */
  void on_connect(modbus::server::session_ptr_t& session, modbus::table& table);

  /**
   * Disconnect callback
   *
   * @param session client session
   * @param table   data table
   */
  void on_disconnect(modbus::server::session_ptr_t& session,
                     modbus::table&                 table);

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

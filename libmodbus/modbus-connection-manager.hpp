#ifndef LIB_MODBUS_MODBUS_CONNECTION_MANAGER_HPP_
#define LIB_MODBUS_MODBUS_CONNECTION_MANAGER_HPP_

#include <set>

#include "modbus-connection.hpp"

namespace modbus {
namespace internal {
class connection_manager {
 public:
  /**
   * Connection map
   */
  typedef std::set<connection::pointer> container;

  /**
   * Delete copy constructor
   */
  connection_manager(const connection_manager&) = delete;

  /**
   * Delete assignment operator
   */
  connection_manager& operator=(const connection_manager&) = delete;

  /**
   * Connection manager constructor
   */
  explicit connection_manager();

  /**
   * Connection manager destructor
   */
  ~connection_manager();

  /**
   * Add the specified connection to the manager and start it
   *
   * @param conn connection pointer
   */
  void start(connection::pointer conn);

  /**
   * Stop the specified connection
   *
   * @param conn connection pointer
   */
  void stop(connection::pointer conn);

  /**
   * Stop all connections
   */
  void stop_all();

 private:
  /**
   * The managed connections
   */
  container connections_;
};
}  // namespace internal
}  // namespace modbus

#endif

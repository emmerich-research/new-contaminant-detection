#ifndef LIB_MODBUS_MODBUS_REQUEST_HANDLER_HPP_
#define LIB_MODBUS_MODBUS_REQUEST_HANDLER_HPP_

#include <string_view>

#include <boost/core/noncopyable.hpp>

#include "modbus-types.hpp"

namespace modbus {
// forward declaration
class table;

class request_handler : private boost::noncopyable {
 public:
  /**
   * Request handler constructor
   *
   * @param data_table data table pointer
   */
  explicit request_handler(table* data_table);
  /**
   * Handle request
   *
   * @param packet request packet
   *
   * @return packet to send
   */
  internal::packet_t handle(const std::string_view& packet);
  /**
   * Handle request
   *
   * @param packet request packet
   *
   * @return packet to send
   */
  internal::packet_t handle(const internal::packet_t& packet);

 private:
  /**
   * Data table pointer
   */
  table* data_table_;
};
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_REQUEST_HANDLER_HPP_

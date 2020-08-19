#ifndef LIB_MODBUS_MODBUS_CONNECTION_HPP_
#define LIB_MODBUS_MODBUS_CONNECTION_HPP_

#include <memory>

#include <boost/asio.hpp>

#include "modbus-types.hpp"

namespace modbus {
namespace internal {
class connection_manager;
class request_handler;
// class request_parser;

class connection : public std::enable_shared_from_this<connection> {
 public:
  /**
   * Connection pointer
   */
  typedef std::shared_ptr<connection> pointer;

  /**
   * Delete copy constructor
   */
  connection(const connection&) = delete;

  /**
   * Delete assignment operator
   */
  connection& operator=(const connection&) = delete;

  /**
   * Construct a connection with the given socket
   *
   * @param socket  socket to use
   * @param manager connection manager
   * @param handler request handler
   */
  explicit connection(boost::asio::ip::tcp::socket socket,
                      connection_manager&          manager,
                      request_handler&             handler);

  /**
   * Start the first asynchronous operation for the connection
   */
  void start();

  /** Stop all asynchronous operations associated with the connection */
  void stop();

 private:
  /**
   * Perform an asynchronous read operation
   */
  void do_read();

  /**
   * Perform an asynchronous write operation
   */
  void do_write();

 private:
  /**
   * Socket for the connection.
   */
  boost::asio::ip::tcp::socket socket_;

  /**
   * The manager for this connection
   */
  connection_manager& connection_manager_;

  /**
   * The handler used to process the incoming request
   */
  request_handler& request_handler_;

  /**
   * Buffer for incoming data
   */
  packet_t buffer_;

  /**
   * The incoming request
   */
  // request request_;

  /**
   * The parser for the incoming request
   */
  // request_parser request_parser_;

  /**
   * The reply to be sent back to the client.
   */
  // reply reply_;
};
}  // namespace internal
}  // namespace modbus

#endif

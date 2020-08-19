#ifndef LIB_MODBUS_SERVER_HPP_
#define LIB_MODBUS_SERVER_HPP_

#include <exception>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "modbus-utilities.hpp"

namespace modbus {
// forward declarations
class session;

class server {
 public:
  /**
   * Server pointer
   */
  typedef std::unique_ptr<server> pointer;

  /**
   * Server create
   */
  MAKE_STD_UNIQUE(server)

 public:
  /**
   * Delete copy constructor
   */
  server(const server&) = delete;

  /**
   * Delete assignment operator
   */
  server& operator=(const server&) = delete;

  /**
   * Server Constructor
   *
   * Will initialize modbus context
   *
   * @param address     address to listen to
   * @param port        port to listen to
   */
  explicit server(const std::string& address, const std::string& port);

  /**
   * Server Destructor
   */
  ~server();

  /**
   * Run server
   */
  void run();

 private:
  /**
   * Start accepting request
   */
  void do_accept();

  /**
   * Handle request to stop server
   */
  void do_await_stop();

 private:
  /**
   * IO context
   */
  boost::asio::io_context io_context_;

  /**
   * The signal_set is used to register for process termination notifications.
   */
  boost::asio::signal_set signals_;

  /**
   * TCP acceptor
   */
  boost::asio::ip::tcp::acceptor acceptor_;
};
}  // namespace modbus

#endif  // LIB_MODBUS_SERVER_HPP_

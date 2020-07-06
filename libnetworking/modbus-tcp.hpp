#ifndef LIB_NETWORKING_MODBUS_TCP_HPP_
#define LIB_NETWORKING_MODBUS_TCP_HPP_

/** @file modbus-tcp.hpp
 *  @brief Modbus TCP implementation
 *
 * Modbus TCP Implementation
 */

#include <cstdint>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <libcore/core.hpp>

#include "modbus.hpp"

NAMESPACE_BEGIN

namespace networking {
// forward declarations
namespace modbus {
class TCP;
}

namespace modbus {
/**
 * @brief modbus::TCP implementation.
 *        This is implementation class of Modbus that is implemented over TCP
 * network
 *
 * Read Modbus_Messaging_Implementation_Guide_V1_0b.pdf for more info
 *
 * @author Ray Andrew
 * @date   July 2020
 */
class TCP : public Modbus, std::enable_shared_from_this<TCP> {
 public:
  /**
   * Create shared_ptr<modbus::TCP>
   *
   * Pass every args to modbus::TCP()
   */
  MAKE_STD_SHARED(TCP)
  /**
   * Create unique_ptr<modbus::TCP>
   *
   * Pass every args to modbus::TCP()
   */
  MAKE_STD_UNIQUE(TCP)
  /**
   * Open connection to Modbus server
   */
  virtual Modbus::ErrorCode connect() override;
  /**
   * Close connection to Modbus server
   */
  virtual Modbus::ErrorCode close() override;
  /**
   * Prepare request before sending
   *
   * @param req       message to be sent
   * @param function  Modbus function
   * @param address   destination address
   * @param quantity  quantity of address
   *
   * @return length of packet
   */
  virtual unsigned int build_request(Modbus::Buffer&         req,
                                     const modbus::function& function,
                                     const std::uint16_t&    address,
                                     const std::uint16_t&    quantity) override;
  /**
   * Send message to Modbus remote device
   *
   * This could be synchronous or asynchronous
   *
   * @param request request to send
   * @param length  length of message
   *
   * @return ModbusResponse / ModbusError
   */
  virtual Modbus::Response send(Modbus::Buffer&    request,
                                const std::size_t& length) override;

 protected:
  /**
   * modbus::TCP Constructor
   *
   * @param host       server hostname / server IP
   * @param port       server port, default 502 based on Modbus TCP/IP
   * implementation (should be more than 0 and less than 65535 (16 bit))
   * @param connect_timeout  connect to server timeout (default: Infinite)
   * @param request_timeout  request timeout (default: Infinite)
   * @param response_timeout response timeout (default: Infinite)
   */
  TCP(const char*            host,
      const char*            port = "502",
      const Modbus::Timeout& connect_timeout = Modbus::MaxTimeout,
      const Modbus::Timeout& request_timeout = Modbus::MaxTimeout,
      const Modbus::Timeout& response_timeout = Modbus::MaxTimeout);
  /**
   * modbus::TCP Constructor
   *
   * @param host       server hostname / server IP
   * @param port       server port, default 502 based on Modbus TCP/IP
   * implementation (should be more than 0 and less than 65535 (16 bit))
   * @param connect_timeout  connect to server timeout (default: Infinite)
   * @param request_timeout  request timeout (default: Infinite)
   * @param response_timeout response timeout (default: Infinite)
   */
  TCP(const char*            host,
      const std::string&     port,
      const Modbus::Timeout& connect_timeout = Modbus::MaxTimeout,
      const Modbus::Timeout& request_timeout = Modbus::MaxTimeout,
      const Modbus::Timeout& response_timeout = Modbus::MaxTimeout);
  /**
   * modbus::TCP Constructor
   *
   * @param host       server hostname / server IP
   * @param port       server port, default 502 based on Modbus TCP/IP
   * implementation (should be more than 0 and less than 65535 (16 bit))
   * @param connect_timeout  connect to server timeout (default: Infinite)
   * @param request_timeout  request timeout (default: Infinite)
   * @param response_timeout response timeout (default: Infinite)
   */
  TCP(const char*            host,
      const std::uint16_t&   port,
      const Modbus::Timeout& connect_timeout = TCP::MaxTimeout,
      const Modbus::Timeout& request_timeout = TCP::MaxTimeout,
      const Modbus::Timeout& response_timeout = TCP::MaxTimeout);
  /**
   * modbus::TCP Destructor
   *
   * Close Modbus TCP socket and instance
   */
  virtual ~TCP() override;
  /**
   * Get Modbus Server hostname
   *
   * @return Modbus server hostname
   */
  inline const char* host() const { return host_; }
  /**
   * Get Modbus Server port
   *
   * @return Modbus server port
   */
  inline const char* port() const { return port_; }
  /**
   * Get Boost Asio TCP socket
   *
   * @return TCP socket
   */
  inline boost::asio::io_context& io_context() { return io_context_; }
  /**
   * Get Boost Asio TCP socket (const)
   *
   * @return TCP socket (const)
   */
  inline const boost::asio::io_context& io_context() const {
    return io_context_;
  }
  /**
   * Get Boost Asio TCP socket
   *
   * @return TCP socket
   */
  inline boost::asio::ip::tcp::socket& socket() { return socket_; }
  /**
   * Get Boost Asio TCP socket (const)
   *
   * @return TCP socket (const)
   */
  inline const boost::asio::ip::tcp::socket& socket() const { return socket_; }
  /**
   * Send request to server
   *
   * Will incorporate timeout
   *
   * @param request  buffer to send
   * @param length   length of request buffer
   * @param ec       error code
   */
  void send_request(Modbus::Buffer&    request,
                    const std::size_t& length,
                    Modbus::ErrorCode& ec);
  /**
   * Get response from server
   *
   * Will incorporate timeout
   *
   * @param response        buffer to write
   * @param response_length length of response
   * @param ec              error code
   */
  void get_response(Modbus::Buffer&    response,
                    std::size_t&       response_length,
                    Modbus::ErrorCode& ec);
  /**
   * Get response from server
   *
   * Will incorporate timeout
   *
   * @param response        buffer to write
   * @param response_length length of response
   * @param ec              error code
   */
  void handle_get_response(const Modbus::ErrorCode& error_code,
                           std::size_t              bytes_transferred,
                           Modbus::Buffer&          response,
                           modbus::phase&           phase,
                           std::size_t&             response_length,
                           std::size_t&             length_to_receive,
                           Modbus::ErrorCode&       ec);
  /**
   * Run I/O task with specified timeout
   *
   * @param timeout task timeout
   * @param ec      error code
   */
  void run_task(const Modbus::Timeout& timeout, Modbus::ErrorCode& ec);

 public:
  /** * Header Length
   *
   * Also known as : Modbus Application Protocol (MBAP)
   *
   * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf (chapter 4
   * section 1 page 5)
   */
  static constexpr unsigned int HEADER_LENGTH = 7;
  /**
   * Header Length
   *
   * Also known as : Modbus Application Protocol (MBAP)
   *
   * Read Modbus_Application_Protocol_Specification_V1_1b3.pdf (chapter 4
   * section 1 page 5)
   */
  static constexpr unsigned int REQ_LENGTH = 12;

 private:
  /**
   * Remote server host to listen to
   */
  const char* host_;
  /**
   * Remote server port or listening to
   */
  const char* port_;
  /**
   * Boost Asio IO context
   */
  boost::asio::io_context io_context_;
  /**
   * Boost Asio TCP socket
   */
  boost::asio::ip::tcp::socket socket_;
};
}  // namespace modbus
}  // namespace networking

NAMESPACE_END

#endif  // LIB_NETWORKING_MODBUS_TCP_HPP_

#include "networking.hpp"

#include "modbus-tcp.hpp"

#include <iostream>
#include <limits>
#include <string>

#include <libutil/util.hpp>

NAMESPACE_BEGIN

namespace networking {
namespace modbus {
// const unsigned int TCP::HEADER_LENGTH = 7;
// const unsigned int TCP::REQ_LENGTH = 12;

TCP::TCP(const char* host, const char* port)
    : Modbus{HEADER_LENGTH},
      host_{host},
      port_{port},
      io_context_{},
      socket_{io_context_} {}

TCP::TCP(const char* host, const std::string& port) : TCP{host, port.c_str()} {}

TCP::TCP(const char* host, const std::uint16_t& port)
    : TCP{host, std::to_string(port).c_str()} {}

TCP::~TCP() {}

boost::system::error_code TCP::connect() {
  LOG_INFO("Connecting to Modbus server, hostname: {} with port: {}", host(),
           port());

  boost::system::error_code ec;
  boost::asio::connect(
      socket(),
      boost::asio::ip::tcp::resolver{io_context()}.resolve(host(), port()), ec);

  io_context().run();

  return ec;
}

boost::system::error_code TCP::close() {
  LOG_INFO("Disconnecting from hostname: {} with port: {}", host(), port());

  boost::system::error_code ec;

  socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  socket().close(ec);
  io_context().stop();

  return ec;
}

unsigned int TCP::build_request(std::uint8_t*           req,
                                const modbus::function& function,
                                const std::uint16_t&    address,
                                const std::uint16_t&    quantity) {
  massert(Modbus::check_function(function), "sanity");

  if (transaction_id() < std::numeric_limits<std::uint16_t>::max()) {
    transaction_id_++;
  } else {
    transaction_id_ = 0;
  }

  // set header of Modbus TCP packet
  // Read Modbus_Messaging_Implementation_Guide_V1_0b.pdf (chapter 3 section 1.3
  // page 5)

  // set trasanction id should be unique each request
  // split into HI/LOW repr
  Modbus::uint16_to_uint8(req, transaction_id(), 0);

  // TCP Protocol should be 0x00
  req[2] = 0;
  req[3] = 0;

  // set length, will be overridden later before sending
  req[4] = 0;
  req[5] = 0;

  // set unit identifier (slave_id)
  req[6] = slave_id();

  // set function in 8 bit
  req[7] = util::to_underlying(function);

  // set address
  // split into HI/LOW repr
  Modbus::uint16_to_uint8(req, address, 8);

  // set quantity
  // split into HI/LOW repr
  Modbus::uint16_to_uint8(req, quantity, 10);

  return REQ_LENGTH;
}

std::variant<ModbusResponse, ModbusError> TCP::send(std::uint8_t*      request,
                                                    const std::size_t& length) {
  /** Calculate length, read Modbus_Messaging_Implementation_Guide_V1_0b.pdf
   * (chapter 3 section 1.3 page 5)
   *
   * MBAP for TCP is defined as request length - header length
   * Header length consists of transaction id (2 bytes) + protocol id (2 bytes)
   * + length (2 bytes) + unit id (1 byte) However, MBAP length includes unit id
   * as well So we need to substract by 6
   */
  const std::uint16_t mbap_length = static_cast<std::uint16_t>(length - 6);

  // set length of mbap
  Modbus::uint16_to_uint8(request, mbap_length, 4);

  // prepare connection error
  boost::system::error_code ec;

  // send data (blocking)
  boost::asio::write(socket(), boost::asio::buffer(request, length), ec);

  // check boost error
  if (ec) {
    LOG_ERROR("Got connection problem: value={}, message={}", ec.value(),
              ec.message());
    return ModbusError{modbus::exception::connection_problem,
                       fmt::format("{}: {}",
                                   Modbus::exception_message(
                                       modbus::exception::connection_problem),
                                   ec.message())
                           .c_str(),
                       /** internal */ true};
  }

  // receive data (blocking)
  std::uint8_t response[Modbus::MAX_MESSAGE_LENGTH];
  std::size_t  response_length = boost::asio::read(
      socket(), boost::asio::buffer(response, Modbus::MAX_MESSAGE_LENGTH), ec);

  if (response_length > Modbus::MAX_MESSAGE_LENGTH) {
    LOG_ERROR(
        "Modbus response size is bigger than maximum message length from "
        "Modbus specification ({})!",
        Modbus::MAX_MESSAGE_LENGTH);
    return ModbusError{modbus::exception::bad_data,
                       Modbus::exception_message(modbus::exception::bad_data),
                       /** internal */ true};
  }

  // check if response is match with our transaction id and TCP protocol
  if (!Modbus::check_confirmation(request, response)) {
    LOG_ERROR("Modbus Confirmation message is invalid!");
    return ModbusError{modbus::exception::bad_data,
                       Modbus::exception_message(modbus::exception::bad_data),
                       /** internal */ true};
  }

  // check exception
  const modbus::exception exception =
      check_exception(request, response, response_length);

  if (exception != modbus::exception::no_exception) {
    const char* message = Modbus::exception_message(exception);
    LOG_ERROR("Got Modbus exception with message {}", message);
    return ModbusError{exception, message};
  }

  // check length of message
  if (!check_length(request, response, response_length)) {
    LOG_ERROR(
        "Mismatch length or number of bytes between request and response");
    return ModbusError{modbus::exception::bad_data,
                       Modbus::exception_message(modbus::exception::bad_data),
                       true};
  }

  LOG_DEBUG("Request and response are OK, returning data");

  // all tests are passed, we can return the right response
  return ModbusResponse{
      /** transaction id */
      Modbus::uint8_to_uint16(response),
      /** protocol */
      Modbus::uint8_to_uint16(response, 2),
      /** length */
      Modbus::uint8_to_uint16(response, 4),
      /** slave id */
      response[6],
      /** function */
      static_cast<modbus::function>(response[7])
      /** raw response */
  };
}
}  // namespace modbus
}  // namespace networking

NAMESPACE_END

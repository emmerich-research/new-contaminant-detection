#include "networking.hpp"

#include "modbus-tcp.hpp"

#include <functional>
#include <iostream>
#include <limits>
#include <string>

#include <boost/lambda/lambda.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>

#include <libutil/util.hpp>

NAMESPACE_BEGIN

namespace networking {
namespace modbus {
TCP::TCP(const char*            host,
         const char*            port,
         const Modbus::Timeout& connect_timeout,
         const Modbus::Timeout& request_timeout,
         const Modbus::Timeout& response_timeout)
    : Modbus{HEADER_LENGTH, connect_timeout, request_timeout, response_timeout},
      host_{host},
      port_{port},
      io_context_{},
      socket_{io_context_} {}

TCP::TCP(const char*            host,
         const std::string&     port,
         const Modbus::Timeout& connect_timeout,
         const Modbus::Timeout& request_timeout,
         const Modbus::Timeout& response_timeout)
    : TCP{host, port.c_str(), connect_timeout, request_timeout,
          response_timeout} {}

TCP::TCP(const char*            host,
         const std::uint16_t&   port,
         const Modbus::Timeout& connect_timeout,
         const Modbus::Timeout& request_timeout,
         const Modbus::Timeout& response_timeout)
    : TCP{host, std::to_string(port).c_str(), connect_timeout, request_timeout,
          response_timeout} {}

TCP::~TCP() {}

Modbus::ErrorCode TCP::connect() {
  Modbus::ErrorCode ec;

  LOG_INFO("Connecting to Modbus server, hostname: {} with port: {}", host(),
           port());

  boost::asio::async_connect(
      socket(),
      boost::asio::ip::tcp::resolver{io_context()}.resolve(host(), port()),
      boost::lambda::var(ec) = boost::lambda::_1);

  // wait until timeout
  run_task(connect_timeout(), ec);

  if (ec) {
    LOG_ERROR("Failed to connect to Modbus server!");
    return ec;
  }

  // massert(ec == 0, "sanity");
  LOG_INFO("Successfully connected to Modbus server!");
  return ec;
}

Modbus::ErrorCode TCP::close() {
  Modbus::ErrorCode ec;

  LOG_INFO("Disconnecting from hostname: {} with port: {}", host(), port());

  socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  socket().close(ec);
  io_context().stop();

  return ec;
}

unsigned int TCP::build_request(Modbus::Buffer&         req,
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

void TCP::send_request(Modbus::Buffer&    request,
                       const std::size_t& length,
                       Modbus::ErrorCode& ec) {
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

  // send data (blocking)
  LOG_DEBUG("Sending data to Modbus Server hostname={}, port={}", host(),
            port());

  boost::asio::async_write(socket(), boost::asio::buffer(request, length),
                           boost::lambda::var(ec) = boost::lambda::_1);

  run_task(request_timeout(), ec);

  if (ec) {
    LOG_ERROR("Failed to send data to server");
  } else {
    LOG_INFO("Successfully send data to server");
  }
}

void TCP::get_response(Modbus::Buffer&    response,
                       std::size_t&       response_length,
                       Modbus::ErrorCode& ec) {
  modbus::phase phase = modbus::phase::function;
  std::size_t   length_to_receive = header_length() + 1;

  // reset to zero
  response_length = 0;

  // receive data (blocking)
  LOG_DEBUG("Getting response from Modbus Server hostname={}, port={}", host(),
            port());

  while (length_to_receive != 0) {
    boost::asio::async_read(
        socket(), boost::asio::buffer(response),
        boost::asio::transfer_exactly(length_to_receive),
        std::bind(&TCP::handle_get_response, this, std::placeholders::_1,
                  std::placeholders::_2, std::ref(response), std::ref(phase),
                  std::ref(response_length), std::ref(length_to_receive),
                  std::ref(ec)));
    // boost::asio::async_read(
    //     socket(), boost::asio::buffer(response),
    //     boost::asio::transfer_exactly(length_to_receive),
    //     [&](const Modbus::ErrorCode& error_code,
    //         std::size_t              bytes_transferred) {
    //       response_length += bytes_transferred;

    //       switch (phase) {
    //         case modbus::phase::function:
    //           length_to_receive =
    //               calculate_next_response_length_after(phase, response);
    //           phase = modbus::phase::meta;
    //           break;

    //         case modbus::phase::meta:
    //           length_to_receive =
    //               calculate_next_response_length_after(phase, response);
    //           if (response_length > Modbus::MAX_MESSAGE_LENGTH) {
    //             LOG_ERROR(
    //                 "Response data size is bad, more than maximum Modbus
    //                 specs "
    //                 "{}",
    //                 Modbus::MAX_MESSAGE_LENGTH);
    //             return;
    //           }
    //           break;
    //       }

    //       DEBUG_ONLY({
    //         LOG_DEBUG("Next will receive={}B, Received={}B, Total={}B",
    //                   length_to_receive, bytes_transferred, response_length);
    //         std::vector<std::uint8_t> vec;
    //         std::copy(response.begin(), response.begin() + response_length,
    //                   std::back_inserter(vec));
    //         LOG_DEBUG("Current Buffer={}", vec);
    //       })
    //     });

    run_task(response_timeout(), ec);

    if (ec) {
      LOG_ERROR("Failed to get full response!");
      return;
    }
  }

  LOG_DEBUG("Done getting response");
}

void TCP::handle_get_response(const Modbus::ErrorCode& error_code,
                              std::size_t              bytes_transferred,
                              Modbus::Buffer&          response,
                              modbus::phase&           phase,
                              std::size_t&             response_length,
                              std::size_t&             length_to_receive,
                              Modbus::ErrorCode&       ec) {
  if (error_code) {
    ec = error_code;
    return;
  }

  LOG_DEBUG(
      "Bytes transferred={}, Phase={} Res Length={} Length to Receive={} ec={}",
      bytes_transferred, phase, response_length, length_to_receive, ec.value());
  response_length += bytes_transferred;

  switch (phase) {
    case modbus::phase::function:
      length_to_receive = calculate_next_response_length_after(phase, response);
      phase = modbus::phase::meta;
      break;

    case modbus::phase::meta:
      length_to_receive = calculate_next_response_length_after(phase, response);
      if ((response_length + length_to_receive) > Modbus::MAX_MESSAGE_LENGTH) {
        LOG_ERROR(
            "Response data size is bad, it is more than maximum Modbus specs "
            "({}B)",
            Modbus::MAX_MESSAGE_LENGTH);
        return;
      }
      break;
  }

  DEBUG_ONLY({
    LOG_DEBUG("Next will receive={}B, Received={}B, Total={}B",
              length_to_receive, bytes_transferred, response_length);
    std::vector<std::uint8_t> vec;
    std::copy(response.begin(), response.begin() + response_length,
              std::back_inserter(vec));
    LOG_DEBUG("Current Buffer={}", vec);
  })
}

Modbus::Response TCP::send(Modbus::Buffer& request, const std::size_t& length) {
  modbus::exception exception;
  Modbus::ErrorCode ec;
  Modbus::Buffer    response;
  std::size_t       response_length = 1024;

  // blocking request
  send_request(request, length, ec);

  // check boost error
  if (ec) {
    LOG_ERROR("Got connection problem: value={}, message={}", ec.value(),
              ec.message());
    return ModbusError{modbus::exception::connection_problem,
                       fmt::format("{}: {}",
                                   Modbus::exception_message(
                                       modbus::exception::connection_problem),
                                   ec.message()),
                       /** internal */ true};
  }

  // response blocking
  get_response(response, response_length, ec);

  if (ec) {
    LOG_ERROR("Got connection problem: value={}, message={}", ec.value(),
              ec.message());
    return ModbusError{modbus::exception::connection_problem,
                       fmt::format("{}: {}",
                                   Modbus::exception_message(
                                       modbus::exception::connection_problem),
                                   ec.message()),
                       /** internal */ true};
  }

  if (response_length > Modbus::MAX_MESSAGE_LENGTH) {
    LOG_ERROR(
        "Modbus response size is bigger than maximum message length from "
        "Modbus specification ({})",
        Modbus::MAX_MESSAGE_LENGTH);
    return ModbusError{modbus::exception::bad_data,
                       Modbus::exception_message(modbus::exception::bad_data),
                       /** internal */ true};
  }

  // check if response is match with our transaction id and TCP protocol
  if (!Modbus::check_confirmation(request, response)) {
    LOG_ERROR("Modbus Confirmation message is invalid");
    return ModbusError{modbus::exception::bad_data,
                       Modbus::exception_message(modbus::exception::bad_data),
                       /** internal */ true};
  }

  // check exception
  exception = check_exception(request, response, response_length);

  if (exception != modbus::exception::no_exception) {
    const char* message = Modbus::exception_message(exception);
    LOG_ERROR("Got Modbus exception with message {}", message);
    return ModbusError{exception, message};
  }

  // check length of message
  if (!check_length(request, response, response_length)) {
    return ModbusError{modbus::exception::bad_data,
                       Modbus::exception_message(modbus::exception::bad_data),
                       true};
  }

  LOG_DEBUG("Request and response are OK, returning data");

  // all tests are passed, we can return the right response
  return ModbusResponse{/** transaction id */
                        Modbus::uint8_to_uint16(response),
                        /** protocol */
                        Modbus::uint8_to_uint16(response, 2),
                        /** length */
                        Modbus::uint8_to_uint16(response, 4),
                        /** slave id */
                        response[6],
                        /** function */
                        static_cast<modbus::function>(response[7]),
                        /** raw request */
                        request,
                        /** raw response */
                        response};
}

void TCP::run_task(const Modbus::Timeout& timeout, Modbus::ErrorCode& ec) {
  // taken from
  // https://www.boost.org/doc/libs/1_73_0/doc/html/boost_asio/example/cpp11/timeouts/blocking_tcp_client.cpp
  // with some modification

  // restart context
  io_context().restart();
  // run for specified timeout
  io_context().run_for(timeout);

  // if async task completed, context should be stopped
  // else timed out
  if (!io_context().stopped()) {
    // send time out error
    ec = boost::asio::error::timed_out;

    // Close the socket to cancel the outstanding asynchronous operation.
    socket().close();

    // Run the io_context again until the operation completes.
    io_context().run();
  }
}
}  // namespace modbus
}  // namespace networking

NAMESPACE_END

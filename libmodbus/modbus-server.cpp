#include "modbus.hpp"

#include "modbus-server.hpp"

#include <signal.h>
#include <utility>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include "modbus-logger.hpp"

#include "modbus-adu.hpp"
#include "modbus-constants.hpp"
#include "modbus-exception.hpp"
#include "modbus-types.hpp"

#include "modbus-bit-read.hpp"

namespace modbus {
server::server(std::size_t concurrency)
    : server_{constants::max_adu_length, constants::max_adu_length,
              concurrency},
      data_table_{},
      req_handler_{&data_table_},
      on_connect_cb_{[](auto&, auto&) {}},
      on_disconnect_cb_{[](auto&, auto&) {}} {
  server_.bind_start(std::bind(&server::on_start, this, std::placeholders::_1))
      .bind_stop(std::bind(&server::on_stop, this, std::placeholders::_1))
      .bind_connect(std::bind(&server::on_connect, this, std::placeholders::_1))
      .bind_disconnect(
          std::bind(&server::on_disconnect, this, std::placeholders::_1))
      .bind_recv(std::bind(&server::on_receive, this, std::placeholders::_1,
                           std::placeholders::_2));
}

server::~server() {
  server_.stop();
}

void server::on_start(asio::error_code ec) {
  logger::get()->debug("starting tcp server @ {} {}, message: {}",
                       server_.listen_address(), server_.listen_port(),
                       ec.message());
}

void server::on_stop(asio::error_code ec) {
  logger::get()->debug("stopping tcp server, message: {}", ec.message());
}

void server::on_connect(session_ptr_t& session_ptr) {
  session_ptr->no_delay(true);
  on_connect_cb_(session_ptr, data_table_);
  logger::get()->debug("client enters: {} {} {} {}",
                       session_ptr->remote_address(),
                       session_ptr->remote_port(), session_ptr->local_address(),
                       session_ptr->local_port());
}

void server::on_disconnect(session_ptr_t& session_ptr) {
  on_disconnect_cb_(session_ptr, data_table_);
  logger::get()->debug("client leaves: {} {} {}", session_ptr->remote_address(),
                       session_ptr->remote_port(), asio2::last_error_msg());
}

void server::on_receive(session_ptr_t&   session_ptr,
                        std::string_view raw_packet) {
  auto response = req_handler_.handle(raw_packet);

#ifndef DEBUG_ON
  logger::get()->debug("[Response, {}]", utilities::packet_str(response));
#endif

  session_ptr->send(response, [](std::size_t bytes_sent) {
    logger::get()->debug("bytes sent {}", bytes_sent);
  });
}

void server::run(std::string_view port) {
  server_.start("0.0.0.0", port);
}
}  // namespace modbus

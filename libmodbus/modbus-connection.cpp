#include "modbus.hpp"

#include "modbus-connection.hpp"

namespace modbus {
namespace internal {
connection::connection(boost::asio::ip::tcp::socket socket,
                       connection_manager&          manager,
                       request_handler&             handler)
    : socket_{std::move(socket)},
      connection_manager_{manager},
      request_handler_{handler} {}

void connection::start() {
  do_read();
}

void connection::stop() {
  socket_.close();
}

void connection::do_read() {
  auto self(shared_from_this());
  socket_.async_read_some(
      boost::asio::buffer(buffer_),
      [this, self](boost::system::error_code ec,
                   std::size_t               bytes_transferred) {
        if (!ec) {
        } else if (ec != boost::asio::error::operation_aborted) {
          // connection_manager_.stop(shared_from_this());
        }
      });
}

void connection::do_write()
{
  auto self(shared_from_this());
  /*boost::asio::async_write(*/
  // socket_, reply_.to_buffers(),
  //[this, self](boost::system::error_code ec, std::size_t) {
  // if (!ec) {
  //// Initiate graceful connection closure.
  // boost::system::error_code ignored_ec;
  // socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
  // ignored_ec);
  //}

  // if (ec != boost::asio::error::operation_aborted) {
  //// connection_manager_.stop(shared_from_this());
  //}
  /*});*/
}
}  // namespace internal
}  // namespace modbus

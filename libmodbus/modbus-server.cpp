#include "modbus.hpp"

#include "modbus-server.hpp"

#include <signal.h>
#include <functional>
#include <utility>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include "modbus-constants.hpp"
#include "modbus-types.hpp"

static void cout_bytes(const modbus::internal::packet_t& packet) {
  modbus::internal::packet_t::size_type index = 0;

  std::string s = "[Packet, ";

  for (unsigned byte : packet) {
    index++;
    if (index < packet.size()) {
      s += fmt::format("{:#04x} ", byte);
    } else {
      s += fmt::format("{:#04x}", byte);
    }
  }

  s += "]";

  std::cout << s << std::endl;
}

namespace modbus {
// namespace exception {
// failed_allocation::failed_allocation(type type__) : type_{type__} {}

// const char* failed_allocation::what() const noexcept {
//   switch (type_) {
//     case context:
//       return "Failed to allocate modbus context";
//     case mapping:
//       return "Failed to allocate modbus mapping";
//     case socket:
//       return "Failed to allocate socket";
//     default:
//       return "Failed to allocate";
//   }
// }

// connection_problem::connection_problem(type type__) : type_{type__} {}

// const char* connection_problem::what() const noexcept {
//   switch (type_) {
//     case closed:
//       return "Socket is not opened";
//     case listen:
//       return "Cannot listen to specified service / port";
//     case select:
//       return "Server select call failure.";
//     default:
//       return "Something wrong with the connection";
//   }
// }
// }  // namespace exception

class session : public std::enable_shared_from_this<session> {
 public:
  /**
   * Session pointer
   */
  typedef std::shared_ptr<session> pointer;

  /**
   * Session factory
   *
   * @param args arguments to pass to constructor
   */
  template <typename... Args>
  inline static auto create(Args&&... args) {
    return pointer(new session(std::forward<Args>(args)...));
  }

 public:
  /**
   * Get socket
   *
   * @return tcp socket
   */
  boost::asio::ip::tcp::socket& socket() { return socket_; }

  /**
   * Start connection
   */
  void start() {
    handle_read();
  }

 private:
  /**
   * Session constructor
   *
   * @param io_context boost asio io context
   */
  session(boost::asio::io_context& io_context) : socket_{io_context} {
    packet_.resize(constants::max_adu_length);
  }

  /**
   * Handle read
   */
  void handle_read() {
    auto self(shared_from_this());

    boost::asio::async_read(
        socket_, boost::asio::buffer(packet_, constants::max_adu_length),
        [this, self](boost::system::error_code error, std::size_t length) {
          if (!error) {
            handle_write(length);
          }
        });
  }

  /**
   * Handle write
   *
   * @param error             error code
   * @param bytes_transferred bytes transferred
   */
  void handle_write(std::size_t length) {
    auto self(shared_from_this());

    cout_bytes(packet_);

    boost::asio::async_write(socket_, boost::asio::buffer(packet_, length),
                             [this, self](boost::system::error_code error,
                                          std::size_t /* length */) {
                               if (!error) {
                                 handle_read();
                               }
                             });
  }

 private:
  /**
   * TCP socket
   */
  boost::asio::ip::tcp::socket socket_;
  /**
   * Packet
   */
  internal::packet_t packet_;
};

server::server(const std::string& address, const std::string& port)
    : io_context_{1}, signals_{io_context_}, acceptor_{io_context_} {
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif  // defined(SIGQUIT)

  do_await_stop();

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_context_);
  boost::asio::ip::tcp::endpoint endpoint =
      *resolver.resolve(address, port).begin();
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  do_accept();
}

server::~server() {}

// void server::prepare_mapping(const helper::map_config& config) {}

void server::run() {
  io_context_.run();
}

void server::do_accept() {
  acceptor_.async_accept([this](boost::system::error_code    ec,
                                boost::asio::ip::tcp::socket socket) {
    if (!acceptor_.is_open()) {
      return;
    }

    if (!ec) {
      /*connection_manager_.start(std::make_shared<connection>(*/
      /*std::move(socket), connection_manager_, request_handler_));*/
    }

    do_accept();
  });
}

void server::do_await_stop() {
  signals_.async_wait([this](boost::system::error_code /*ec*/, int /*signo*/) {
    // The server is stopped by cancelling all outstanding asynchronous
    // operations. Once all operations have finished the io_context::run()
    // call will exit.
    acceptor_.close();
    // connection_manager_.stop_all();
  });
}
}  // namespace modbus

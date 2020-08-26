#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <boost/asio/io_context.hpp>

#include <libcore/core.hpp>
#include <libmodbus/modbus.hpp>

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

  LOG_DEBUG("{}", s);
}

class server_logger : public modbus::logger {
 public:
  explicit server_logger(bool debug = false) : modbus::logger(debug) {}

  virtual ~server_logger() override {}

  inline virtual void log(const std::string& message) const noexcept override {
    LOG_DEBUG("{}", message);
  }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  USE_NAMESPACE

  if (initialize_core()) {
    std::cerr << "cannot initialize config, state, and logger!" << std::endl;
    return ATM_ERR;
  }

  modbus::logger::create<server_logger>(true);

  auto server = modbus::server::create_unique();

  server->bind_connect(
      [](auto& session_ptr, [[maybe_unused]] auto& data_table) {
        session_ptr->start_timer(1, std::chrono::seconds(1),
                                 []() { /* creating heartbeat */ });
      });

  server->run("1502");

  while (std::getchar() != '\n') {
  }

  return 0;
}

#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <utility>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <boost/asio/io_context.hpp>

#include <libcore/core.hpp>
#include <libmodbus/modbus.hpp>

class server_logger : public modbus::logger {
 public:
  explicit server_logger(bool debug = false) : modbus::logger(debug) {}

  virtual ~server_logger() override {}

  inline virtual void error(
      const std::string& message) const noexcept override {
    LOG_ERROR("{}", message);
  }

  inline virtual void debug(
      const std::string& message) const noexcept override {
    if (debug_) {
      LOG_DEBUG("{}", message);
    }
  }

  inline virtual void info(const std::string& message) const noexcept override {
    LOG_INFO("{}", message);
  }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  USE_NAMESPACE

  if (initialize_core()) {
    std::cerr << "cannot initialize config, state, and logger!" << std::endl;
    return ATM_ERR;
  }

  modbus::logger::create<server_logger>(true);

  auto&& data_table = modbus::table::create();
  auto&& server = modbus::server::create(std::move(data_table));

  server->bind_connect(
      []([[maybe_unused]] auto& session_ptr, [[maybe_unused]] auto& table) {
        /*session_ptr->start_timer(1, std::chrono::seconds(1), [&table]() {*/
        // LOG_INFO("Coils addr 0x00: {}",
        // table.coils().get(modbus::address_t{0x00}));
        /*});*/
      });

  server->run();

  while (std::getchar() != '\n') {
  }

  return 0;
}

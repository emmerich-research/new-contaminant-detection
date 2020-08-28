#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include <boost/asio.hpp>

#include <asio2/config.hpp>
#include <asio2/version.hpp>

#include <asio2/base/timer.hpp>
#include <asio2/tcp/tcp_client.hpp>

#include <libcore/core.hpp>
#include <libmodbus/modbus.hpp>
#include <libutil/util.hpp>

static void cout_bytes(const modbus::packet_t& packet) {
  LOG_DEBUG("[Packet, {}]", modbus::utilities::packet_str(packet));
}

class client_logger : public modbus::logger {
 public:
  explicit client_logger(bool debug = false) : modbus::logger(debug) {}

  virtual ~client_logger() override {}

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

int main(int argc, char* argv[]) {
  USE_NAMESPACE

  try {
    if (argc != 3) {
      std::cerr << "Usage: tcp_client <host> <port>\n";
      return 1;
    }

    if (initialize_core()) {
      std::cerr << "cannot initialize config, state, and logger!" << std::endl;
      return ATM_ERR;
    }

    modbus::logger::create<client_logger>(true);
    asio2::tcp_client client;

    /*modbus::request::read_coils req(*/
    // modbus::address_t{0x00},
    //// modbus::num_bits_t{modbus::num_bits_t::constant<0x7D0>{}}
    /*modbus::read_num_bits_t{0x7D0});*/

    /*modbus::request::write_single_coil req;*/
    modbus::request::write_multiple_coils req(
        modbus::address_t{0x00}, modbus::write_num_bits_t{2}, {true, true});
    req.initialize({0x1234, 0x01});

    auto request = req.encode();

    client.auto_reconnect(true, std::chrono::milliseconds(1000));
    // client.start_timer(1, std::chrono::seconds(1), []() {});
    client
        .bind_connect([&]([[maybe_unused]] asio::error_code ec) {
          if (asio2::get_last_error()) {
            LOG_DEBUG("connect failure : {} {}", asio2::last_error_val(),
                      asio2::last_error_msg());
          } else {
            LOG_DEBUG("connect success : {} {}", client.local_address(),
                      client.local_port());
          }

          cout_bytes(request);
          client.send(request);
        })
        .bind_disconnect([]([[maybe_unused]] asio::error_code ec) {
          LOG_DEBUG("disconnect : {} {}", asio2::last_error_val(),
                    asio2::last_error_msg());
        })
        .bind_recv([&](std::string_view packet) {
          try {
            // modbus::response::read_coils response(&req);
            // modbus::response::write_single_coil response(&req);
            modbus::response::write_multiple_coils response(&req);
            response.decode(packet);
          } catch (const modbus::ex::specification_error& exc) {
            LOG_ERROR("Modbus exception occured {}", exc.what());
          } catch (const modbus::ex::base_error& exc) {
            LOG_ERROR("Internal exception occured {}", exc.what());
          } catch (const std::exception& exc) {
            LOG_ERROR("Unintended exception occured {}", exc.what());
          }
        });

    // client.start(argv[1], argv[2]);
    client.async_start(argv[1], argv[2]);

    while (std::getchar() != '\n') {
    }

    client.stop();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

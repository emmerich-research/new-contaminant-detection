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

    asio2::tcp_client client;

    modbus::request::read_coils req_read_coils{modbus::address_t{0x00},
                                               modbus::num_bits_t{1}};

    req_read_coils.initialize({0x1234, 0x01});

    auto request = req_read_coils.encode();

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
        .bind_recv([&](std::string_view sv) {
          modbus::internal::packet_t packet{sv.begin(), sv.end()};
          cout_bytes(packet);
          modbus::response::read_coils response{&req_read_coils};
          response.decode(packet);
          LOG_DEBUG("{}", response.bits());
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

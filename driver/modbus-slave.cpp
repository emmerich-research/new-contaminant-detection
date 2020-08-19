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

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  USE_NAMESPACE


  if (initialize_core()) {
    std::cerr << "cannot initialize config, state, and logger!" << std::endl;
    return ATM_ERR;
  }

  modbus::table data_table;

  modbus::request::read_coils req_read_coils{modbus::address_t{0x00},
                                             modbus::num_bits_t{1}};

  req_read_coils.initialize({0x1234, 0x01});

  auto packet = req_read_coils.encode();

  LOG_DEBUG("{}", req_read_coils);
  cout_bytes(req_read_coils.encode());

  auto res_read_coils = req_read_coils.execute(data_table);

  LOG_DEBUG("{}", *res_read_coils);

  cout_bytes(res_read_coils->encode());

  auto server = modbus::server::create_unique("localhost", "1502");

  server->run();

  LOG_DEBUG("Shutting down completed");

  return 0;
}
